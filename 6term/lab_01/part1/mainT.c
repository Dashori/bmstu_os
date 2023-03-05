#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHILDS_QTY 5

void child_routine(int index, int* sockets)
{
    char buf[1024];
    char msg[] = "bb 0";
    msg[3] += index;

    close(sockets[1]);
    printf("MSG sent by child %d: %s \n", index, msg);
    write(sockets[0], msg, sizeof(msg));
    read(sockets[0], buf, sizeof(buf));
    printf("MSG received by child %d: %s \n", index, buf);
    close(sockets[0]);
}


int main()
{
    int sockets[CHILDS_QTY][2];
    char buf[1024];
    pid_t childpid;
    char p_msg[] = "aa 0";

    for (int i = 0; i < CHILDS_QTY; i++)
    {
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0)
        {
            perror("socketpair() failed");
            return EXIT_FAILURE;
        }
    }


    for (size_t i = 0; i < CHILDS_QTY; ++i)
    {
        switch (childpid = fork())
        {
        case -1:
            perror("fork error");

            exit(1);
            break;

        case 0:
            child_routine(i, sockets[i]);

            return EXIT_SUCCESS;
        }
    }


    for (int i = 0; i < CHILDS_QTY; i++)
    {
        close(sockets[i][0]);
        read(sockets[i][1], buf, sizeof(buf));
        printf("MSG received by parent: %s \n", buf);
        sleep(3);
        write(sockets[i][1], p_msg, sizeof(p_msg));
        printf("MSG sent by parent: %s \n", p_msg);
        p_msg[3]++;
        close(sockets[i][1]);
        //sleep(3);
    }
    

    return EXIT_SUCCESS;
}
