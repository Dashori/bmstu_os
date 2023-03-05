#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <string.h>

#define N 64

char *prod;
char *cons;
char *current_char;
struct sembuf producer_enter_cr[2] = { {1, -1, 0},
 									   {2, -1, 0} };

struct sembuf producer_exit_cr[2] = { {2, 1, 0},
									  {0, 1, 0} };

struct sembuf consumer_enter_cr[2] = { {0, -1, 0},
									   {2, -1, 0} };

struct sembuf consumer_exit_cr[2] = { {2, 1, 0},
									  {1, 1, 0} };

void buffer_init(char* addr);
void write_buffer(char **addr, char symb);
void read_buffer(char **addr, char *dst);

void producer_create(char* const buffer, const int prod_id, const int sem_id);
void producer_run(char* buffer, int sem_id, int prod_id);

void consumer_run(char* const buffer, const int sem_id, const int cons_id);
void consumer_create(char* const buffer, const int con_id, const int sem_id);

int main(void)
{
	setbuf(stdout, NULL);
	srand(time(NULL));
	int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	key_t key = ftok("pcfile1", 3); // преобразовывает имя файла и идентификатор проекта в ключ для системных вызовов  
	
	if (key == -1)
	{
		perror("ftok error");
		exit(1);
	}

	int shmid = shmget(key, 256, IPC_CREAT | perms); // создаем разделяемую память
	
	if (shmid == -1)
	{
		perror("shmget error");
		exit(1);
	}

	char *addr = shmat(shmid, NULL, 0); // подключаем сегмент общей памяти shmid к адресному пространству вызывающего процесса

	if (addr == (char*)-1)
	{
		perror("shmat error");
		exit(1);
	}

	current_char = addr + 80;

	buffer_init(addr);
	int sem_fd =  semget(key, 3, IPC_CREAT | perms); // создаем новый набор семафоров 

	if (sem_fd == -1)
	{
		perror("semget error");
		exit(1);
	}
	
	// инициализируем семафоры

	if (semctl(sem_fd, 2, SETVAL, 1) == -1)  // бинарный семафор
	{
		perror("semctl error 2");
		exit(1);
	}
	if (semctl(sem_fd, 1, SETVAL, N) == -1) // количество пустых 
	{
		perror("semctl error 1");
		exit(1);
	}
	if (semctl(sem_fd, 0, SETVAL, 0) == -1) // количество для потребителя 
	{
		perror("semctl error 0");
		exit(1);
	}


	for (int i = 0; i < 3; i++)
		producer_create(addr, i + 1, sem_fd);

	for (int i = 0; i < 3; i++)
		consumer_create(addr, i + 1, sem_fd);

	for (size_t i = 0; i < 6; i++)
	{
		int status;

		if (wait(&status) == -1)
		{
			perror("wait error");
			exit(1);
		}

		if (!WIFEXITED(status))
			printf("One of children terminated abnormally\n");
	}

	if (shmctl(shmid, IPC_RMID, NULL) ||  (shmdt((void*)addr) == -1) || (semctl(sem_fd, 0, IPC_RMID, 0) == -1))  // помечаем сегмент как удаленный
	{
		perror("error");
		exit(1);
	}
	
	return 0;
}


void buffer_init(char* addr)
{
	char **b = (char**) addr;
	*b = *(b + 1) = addr + (2 * sizeof(char*));
	*current_char = 'a';
}

void producer_create(char* const buffer, const int prod_id, const int sem_id)
{
	pid_t childpid;

	if ((childpid = fork()) == -1)
	{
		perror("Producer fork error");
		exit(1);
	}
	else if (childpid == 0)
	{
		for (int i = 0; i < 8; i++)
			producer_run(buffer, sem_id, prod_id);
		exit(0);
	}
}

void producer_run(char* buffer, int sem_id, int prod_id)
{
	int sleep_time = rand() % 2 + 1;
	sleep(sleep_time);

	int rc = semop(sem_id, producer_enter_cr, 2);

	if (rc == -1)
	{
		perror("Producer enter semop error");
		exit(1);
	}

	char s = (*current_char)++;
	write_buffer((char**)buffer, s);
	printf(" \e[1;31mProducer #%d \tput: \t%c \tsleep: %d\e[0m \n", prod_id, s, sleep_time);
	
	rc = semop(sem_id, producer_exit_cr, 2);

	if (rc == -1)
	{
		perror("Producer exit semop error");
		exit(1);
	}
}

void write_buffer(char **addr, char symb)
{
	prod = *addr;
	*prod = symb;
	*addr = ++prod; 	
}

void read_buffer(char **addr, char *dst)
{
	cons = *(addr + 1);
	*dst = *cons;
	*(addr + 1) = ++cons; 
}


void consumer_create(char* const buffer, const int con_id, const int sem_id)
{
	pid_t childpid;

	if ((childpid = fork()) == -1)
	{
		perror("Consumer for error");
		exit(1);
	}
	else if (!childpid)
	{
		for (int i = 0; i < 8; i++)
			consumer_run(buffer, sem_id, con_id);
		exit(0);
	}
}

void consumer_run(char* const buffer, const int sem_id, const int cons_id)
{
	char ch;
	int sleep_time = rand() % 4 + 1;
	sleep(sleep_time);
	
	int rc = semop(sem_id, consumer_enter_cr, 2);

	if (rc == -1)
	{
		perror("Consumer enter semop error");
		exit(1);
	}

	read_buffer((char**)buffer, &ch);
	printf(" \e[1;34mConsumer #%d \tget:  \t%c \tsleep: %d\e[0m\n", cons_id, ch, sleep_time);
	
	rc = semop(sem_id, consumer_exit_cr, 2);

	if (rc == -1)
	{
		perror("Consumer exit semop error");
		exit(1);
	}
}

