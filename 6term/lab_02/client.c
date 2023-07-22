#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>

#define BUF_SIZE 50

int sockfd;

void signal_handler(int signal)
{
    printf("\nCaught signal = %d\n", signal);
    close(sockfd);
    printf("\nClient exiting.\n");
    exit(0);
}

int main(int argc, char ** argv)
{
    if ((signal(SIGINT, signal_handler) == SIG_ERR)) {
        perror("can't attach handler\n");
        return EXIT_FAILURE;
    }

    if (argc != 3)
    {
        perror("args not enough\n");
        return EXIT_FAILURE;
    }

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        perror("host not found\n");
        return EXIT_FAILURE;
    }

    short port = (short) atoi(argv[2]);

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr = *((struct in_addr*) server->h_addr_list[0]);
    sa.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("socket() failed");
        return EXIT_FAILURE;
    }

    if (connect(sockfd, (struct sockaddr*) &sa, sizeof(sa)) < 0)
    {
        perror("connect() failed");
        return EXIT_FAILURE;
    }

    char buf[BUF_SIZE];
    while (1) 
    {
        if (scanf("%s", buf) != 1)
        {
            perror("scanf() failed");
            return EXIT_FAILURE;
        }
        sprintf(buf, "%s %d", buf, getpid());
        write(sockfd, buf, sizeof(buf));
        read(sockfd, buf, sizeof(buf));
        printf("\nreceived message: %s\n\n", buf);
    }

    return EXIT_SUCCESS;
}