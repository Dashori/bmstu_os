#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TIME_FOR_SLEEP 3

int main()
{
    pid_t children[2];

    for (int i = 0; i < 2; i++)
    {
        if ((children[i] = fork()) == -1)
        {
            perror("Can't fork\n");

            exit(1);
        }
        else if (children[i] == 0)
        {
            printf("\nBefore sleep: Child %d: pid = %d, ppid = %d, pgrp = %d\n", i + 1, getpid(), getppid(), getpgrp());

            sleep(TIME_FOR_SLEEP);

            printf("\nAfter sleep: Child %d: pid = %d, ppid = %d, pgrp = %d\n", i + 1, getpid(), getppid(), getpgrp());
            
            return 0;
        }
        else
        {
            printf("Parent: pid = %d, pgrp = %d, child = %d\n", getpid(), getpgrp(), children[i]);
        }
    }

    return 0;
}