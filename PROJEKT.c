#include <stdio.h>
#include <unistd.h>
#include <signal.h>  
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/types.h>    
#include <sys/stat.h>
#define READ  	 0  
#define WRITE  	 1

int qid;
FILE *file;                       
key_t msgkey;                 
pid_t pid_0, pid_1, pid_2, pid_3, gpid; 
int isSuspended=0;		
const char *fifoNames[3];
int receivedSignal = 0;
int receivedSignalNum;
int processNumber;



struct mymsgbuf {
    long    mtype;         
    int 	liczba; 		
} msg;

struct mymsgbuf buf;


char *fgetstr(char *string, int n, FILE *stream)
{
	char *result;
	result = fgets(string, n, stream);
	if(!result)
		return(result);

	if(string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = 0;

	return(string);
}


int open_queue( key_t keyval ) {
    int     qid;     
    if((qid = msgget( keyval, IPC_CREAT | 0660 )) == -1)
        return(-1);
return(qid);
}


int remove_queue( int qid ){
    if( msgctl( qid, IPC_RMID, 0) == -1)
        return(-1);      
    return(0);
}


int send_message( int qid, struct mymsgbuf *qbuf ){
    int result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);        
    if((result = msgsnd( qid, qbuf, length, 0)) == -1)
          return(-1);
return(result);
}


int read_message( int qid, long type, struct mymsgbuf *qbuf ){
    int     result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);        
    if((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
        return(-1);  
    return(result);
}


void MENU(void){
	printf("\x1b[7mMENU:\x1b[0m\n 1. tryb interaktywny\n 2. odczyt z pliku\n 3. odczyt danych z /dev/urandom\n\x1b[7mOPCJA:\x1b[0m ");
}


void externalSignalHandler(int signum)
{
	receivedSignal = 1;
	receivedSignalNum = signum;
	killpg(gpid, SIGUSR2);
}


void internalSignalHandler(int signum)
{
	int fifo;
	int externalSignalNum;

	if (receivedSignal)
	{
		int i = 0;
		externalSignalNum = receivedSignalNum;


		for (i = 1; i <= 3; i++)
		{
			if (i != processNumber)
			{
				fifo = open(fifoNames[i - 1], O_WRONLY);
				write(fifo, &externalSignalNum, sizeof(int));
				close(fifo);
			}
		}
		receivedSignal = 0;
		if (externalSignalNum == SIGUSR1)
			kill(gpid, SIGUSR1);
	}

	else
	{
		fifo = open(fifoNames[processNumber - 1], O_RDONLY);
		while (1)
		{
			if (read(fifo, &externalSignalNum, sizeof(int)) > 0)
				break;
		}
		close(fifo);
	}

	switch (externalSignalNum)
	{
	case SIGUSR1:
		exit(0);
		break;
	case SIGINT:
		isSuspended = 1;
		break;
	case SIGCONT:
		isSuspended = 0;
		break;
	}
}


void czyszczenie()
{
	int i;
	for (i = 0; i < 3; i++)
	{
		unlink(fifoNames[i]);
	}
	remove_queue(qid);
	exit(0);
}


void Signals()
{
	signal(SIGUSR1, externalSignalHandler);
	signal(SIGINT, externalSignalHandler);
	signal(SIGCONT, externalSignalHandler);
	signal(SIGUSR2, internalSignalHandler);
}


void parentSignalHandler()
{
}


void ParentSignals()
{
	signal(SIGUSR2, parentSignalHandler);
	signal(SIGUSR1, czyszczenie);
}








int main(void){
int desk[2]; 
pipe(desk);
int odp; 
char znak; 
char string[200]; 
int i=0;
int suma=0; 
int licznik_linia=0; 
char fileName[256]; 
int kolor=1;

if ((msgkey = ftok(".", 'D'))==-1)
	{
		perror("ERROR ftok"); 
		exit(1);
	}
	
if (( qid = open_queue(msgkey)) == -1)
	{ 
        perror("ERROR kolejka");
        exit(1);
    }
    
														
																		fifoNames[0] = "fifo1";
																		fifoNames[1] = "fifo2";
																		fifoNames[2] = "fifo3";

																		mkfifo(fifoNames[0], 0666);
																		mkfifo(fifoNames[1], 0666);
																		mkfifo(fifoNames[2], 0666);
																		gpid = getpid();


/* menu */

system("clear");
MENU();
scanf("%d%*c",&odp);
printf("\n");
fprintf(stdout,"Initiate <%d>\n", getpid());



/* P3 */

pid_3=fork();
if(pid_3==0){
  
  
																		setpgid(getpid(), gpid);
																		processNumber = 3;
																		Signals();
   
            buf.mtype   = 1;     
   
     
            fprintf(stdout,"Proces 3 <%d>\n", getpid());
   
			sleep(2);
            while(1)
            {
									
						if(isSuspended == 0)
						{
								read_message(qid, msg.mtype, &buf);
								fprintf(stderr, "\x1b[3%dm %d \x1b[0m", kolor,buf.liczba);
								kolor=(kolor+1)%10;
								licznik_linia++;
									if(licznik_linia==15)
									{
										printf("\n");
										licznik_linia=0;
									}
						}				
						

			}
            
            
            }
            else
            {
/* P2 */
pid_2=fork();                
if(pid_2==0)
{


															setpgid(getpid(), gpid);
															processNumber = 2;
															Signals();
msg.mtype   = 1;     

close(desk[WRITE]);
fprintf(stdout,"Proces 2 <%d>\n", getpid());

		while(1)
		{

					if(isSuspended == 0)
					{			
					if(read(desk[READ],string, sizeof(string))<0){	perror("ERROR PIPE READ");	exit(1); }

						for(i=0;i<strlen(string);i++)
						{

							znak=string[i];
				
							
				
									if(isalpha(znak)!=0)
									{
										suma++;
									}
					
									
									if
									( 
									(string[i]==-59 && (string[i+1]==-101 || string[i+1]==-102 || (string[i+1]<=-68 && string[i+1]>=-71) || (string[i+1]>=-127 && string[i+1]<=-124)))||
									(string[i]==-60 && (string[i+1]==-103 || string[i+1]==-104 || (string[i+1]>=-124 && string[i+1]<=-121)))||
									(string[i]==-61 && (string[i+1]==-77 || string[i+1]==-109))
									) suma++;
									
		
	
						}

					msg.liczba = suma;
					send_message(qid, &msg);
					suma=0;
					}
		}    



}
            else
            {

/* P1 */

pid_1=fork();
if(pid_1==0)
{

															setpgid(getpid(), gpid);
															processNumber = 1;
															Signals();

close(desk[READ]); 
fprintf(stdout,"Proces 1 <%d>\n", getpid());


switch(odp){
	
			case 1: // WPROWADANIE DANYCH Z KLAWIATURY
					usleep(20000);
					fprintf(stdout, "\n------> WPROWADZANIE DANYCH Z KLAWIATURY");
					while(1)
					{
						if(isSuspended == 0)
						{
							
							usleep(20000);
							fprintf(stdout, "\n$ ");
							fgetstr(string, sizeof(string), stdin);
			  
							if(write(desk[WRITE], string, sizeof(string))<0){	perror("ERROR PIPE WRITE");	exit(1); }
							
						}
					}
			break;
			
			case 2:  // ODCZYT DANYCH Z PLIKU
					usleep(20000);
					fprintf(stdout, "\n------> ODCZYT DANYCH Z PLIKU\nPodaj nazwe pliku: ");
					scanf("%s", fileName);
					file=fopen(fileName, "r");
					if(!file)
					{
							fprintf(stderr, "ERROR %s NIE ISTNIEJE", fileName);
							exit(0);
					}   
					else
					{
							fprintf(stdout, "Udalo sie otworzyc plik.\n");
							while(1)
							{
								if(isSuspended == 0)
								{
										while(fgets(string,sizeof(string),file)!=NULL)
										{
										
												if(write(desk[WRITE], string, sizeof(string))<0){	perror("ERROR PIPE WRITE");	exit(1); }
											
										}
								}	
							}
					}
					fclose(file);
			break;
			
			case 3: // ODCZYT DANYCH Z DEV/URANDOM
					usleep(20000);	
					fprintf(stdout, "\n------> ODCZYT DANYCH Z \"/dev/urandom\"\n");
					file=fopen("/dev/urandom","r");
					if(!file)
					{
							fprintf(stderr, "ERROR /dev/urandom");
							exit(0);
					}   
					else
					{
							while(1)
							{
								if(isSuspended == 0)
								{
									while(fgets(string,sizeof(string),file)!=NULL)
									{
										if(write(desk[WRITE], string, sizeof(string))<0){	perror("ERROR PIPE WRITE");	exit(1); }									
									}
								}
							}
					}
					fclose(file);

			break;
			
			default:
			usleep(20000);
			fprintf(stderr, "Nie ma takiej opcji.");
			break;

		  }

}
			else
			{

/* P0 */


																		processNumber = 0;
																		ParentSignals();
					while (1)
					{
					}

			}
		}
	}
return 0;
}

