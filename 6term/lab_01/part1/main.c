#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define CHILD 5

int main()
{
    int sockets[2];
    char buf[32];
    int pid;

    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sockets) < 0)
    {
        perror("socketpair() failed");
        return EXIT_FAILURE;
    }


    for (int i = 0; i < CHILD; i++)
    {
        pid = fork();

        if (pid == -1)
        {
            perror("fork error");
            exit(1);
        }
        else if (pid == 0)
        {   
            char message[14];
            sprintf(message, " %d", getpid()); // 14
            close(sockets[1]);
            printf("Child sent: %s\n", message);
            write(sockets[0], message, sizeof(message));
            sleep(1);
            read(sockets[0], buf, sizeof(buf));
            printf("Child recieved: %s\n", buf);
           // close(sockets[0]);    

            return EXIT_SUCCESS;
        }
    }

    close(sockets[0]);
    for (int i = 0; i < CHILD; i++)
    {
        char message[32];
      
        close(sockets[0]);
        read(sockets[1], buf, sizeof(buf));
        sprintf(message, "buf %s - %d", buf, getpid()); // 14
        printf("Parent sent: %s\n", message);
       // sleep(1);
        write(sockets[1], message, sizeof(message));
        printf("Parent recieved: %s\n", buf);
    }

    close(sockets[1]);

    return EXIT_SUCCESS;
}