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
    int sockets[CHILD][2];
    char buf[1024];
    int pid;

    for (int i = 0; i < CHILD; i++)
    {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0)
        {
            perror("socketpair() failed");
            return EXIT_FAILURE;
        }
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
            char message[] = "CCC 0";
            message[4] += i;
            close(sockets[i][1]);
            printf("Child sent: %s\n", message);
            write(sockets[i][0], message, sizeof(message));
            read(sockets[i][0], buf, sizeof(buf));
            printf("Child recieved: %s\n", buf);
            close(sockets[i][0]);    

            return EXIT_SUCCESS;
        }
        else
        {
            char message[] = "PPP 0";
            message[4] += i;
            close(sockets[i][0]);
            read(sockets[i][1], buf, sizeof(buf));
            printf("Parent sent: %s\n", message);
            sleep(2);
            write(sockets[i][1], message, sizeof(message));
            printf("Parent recieved: %s\n", buf);
            close(sockets[i][1]);
        }

    }

    return EXIT_SUCCESS;
}