#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define TIME_FOR_SLEEP 3

int main()
{   
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
            printf("\nChild %d: pid = %d, ppid = %d, pgrp = %d\n", i + 1, getpid(), getppid(), getpgrp());
        }
        else
        {
            printf("Parent: pid = %d, pgrp = %d, child = %d\n", getpid(), getpgrp(), children[i]);

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

    return 0;
}



