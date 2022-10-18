#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int flag = 0;

void sig_handler(int sig_n)
{
    flag = 1;
	printf("\n СATCH %d\n", sig_n);
}

int main(void)
{
    signal(SIGINT, sig_handler);
 	int fd[2];

	if (pipe(fd) == -1)
	{
	    perror("Pipe error\n");

	    exit(1);
	}

	char message[2][100] = { "xxxsdfgrbvvvvvvvvvvvvvvvxx\n", "yyysdyaf\n" };
    
    pid_t children[2];
    int status;

	for (int i = 0; i < 2; i++)
	{
        if ((children[i] = fork()) == -1)
        {
            perror("Can't fork\n");

            exit(1);
        }
        else if (children[i] == 0)
        {
            printf("Child %d, group %d, parent %d\n", getpid(), getpgrp(), getppid());
            sleep(2);

            if (flag)
            {
                close(fd[0]);
                write(fd[1], message[i], strlen(message[i]));
                printf("%d sent message %s\n", getpid(), message[i]);
            }
            else
            {
                printf("No signal.\n");
                return 0;
            }
           
	        return 0;
	    }
        else
        {
            children[i] = waitpid(children[i], &status, WCONTINUED);

            if (children[i] == -1)
            {
                perror("Can't wait\n");

                exit(1);
            }

            printf("Child %d finished, status: %d\n", children[i], status);

            if (WIFEXITED(status))
                printf("Child finished, code: %d\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf("Child finished, signal %d\n", WTERMSIG(status));
            else if (WIFSTOPPED(status))
                printf("Child stopped, signal %d\n", WSTOPSIG(status));
        }
	}

    if (flag)
    {
        printf("Message receive \n");
        close(fd[1]);
    
        if (read(fd[0], message[0], sizeof(message[0])) == -1)
            return 1;
        
        printf("%s", message[0]);
    }
   

    return 0;
}


// системный вызов fork с оптимизацией, wait, waitpid и процессы сироты и зомби