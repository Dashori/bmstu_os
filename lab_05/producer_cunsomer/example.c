#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

const int MY_FLAG = IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
const int num = 10;
const int count = 15;

int* buf;
int* pos;

#define POTR  (0)
#define PROIS (1)
#define BIN   (2)

struct sembuf start_pr[2] = {{PROIS,-1,0},{BIN,-1,0}};
struct sembuf stop_pr[2]  = {{POTR,1,0},{BIN,1,0}};
struct sembuf start_po[2] = {{POTR,-1,0},{BIN,-1,0}};
struct sembuf stop_po[2]  = {{PROIS,1,0},{BIN,1,0}};

int main()
{
      
	key_t mem_key;
	key_t my_key = ftok("my_file",0);

 	int shmid, semid, status; 

	if ((shmid = shmget(mem_key, (num + 1)*sizeof(int), MY_FLAG)) == -1)
	{
		printf("Error in shmget\n");
		return 1;
	}

	pos = shmat(shmid, 0, 0); 
	buf = pos + sizeof(int);
	
	(*pos) = 0;

	if (*buf == -1)
	{
		printf("Error in shmat\n");
		return 1;
	}

	if ((semid = semget(my_key, 3, MY_FLAG)) == -1)
	{
		printf("Error in semget\n");
		return 1;
	}

	semctl(semid, 2, SETVAL, 0);
	semctl(semid, 1, SETVAL, num);  
	semctl(semid, 2, SETVAL, 1); 	
	
	srand(time(NULL));
	
	pid_t pid;
	if ((pid = fork()) == -1)
	{
		printf("Error in fork\n");
		return 1;
	}

	int k = 0;
    
	switch(pid == 0)
	{
	  case 0:
	    while(k < count)
		{
			semop(semid, start_pr, 2); 
			buf[*pos] = 1+rand()%10;
			printf("\tProisvodstvo^ [%d] -- [%d]\n", k, buf[*pos]);
			(*pos)++;
			semop(semid, stop_pr, 2);
			sleep(rand()%2);
			k++;
		}
	break;
	  default:
		while (k < count)
		{
			semop(semid, start_po, 2);
			(*pos)--;
			printf("Potreblenie^  [%d] -- [%d]\n", k, buf[*pos]);
			semop(semid, stop_po, 2);
			sleep(rand()%2);
			k++;
		}
	break;
	}
	if (pid != 0) 
	{
		if (shmdt(pos) == -1) 
		{
			printf("Error in shmdt\n");
			return 1;
		}
         wait(&status);
	}
	return 0;
}