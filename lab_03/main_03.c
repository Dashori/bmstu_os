#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

int main()
{
    char *exe[2] = { "./lev.exe", "./app.exe" }; //sort.exe

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
            printf("Child pid %d, pgrp %d, ppid %d\n", getpid(), getpgrp(), getppid());

	    	if (execl(exe[i], exe[i], NULL) == -1)
            {
	    	    perror("Error exec");

                exit(1);
            }
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
