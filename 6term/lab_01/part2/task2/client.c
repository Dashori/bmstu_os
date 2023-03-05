#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

int main()
{
	int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

	if (sockfd < 0)
	{
		perror("Create socket error");
		return EXIT_FAILURE;
	}

	struct sockaddr client;
	client.sa_family = AF_UNIX;
    char buf[BUF_SIZE];
    sprintf(buf, "%d", getpid()); // 14
    strcpy(client.sa_data, buf);

    if (bind(sockfd, &client, strlen(client.sa_data) + 1 + sizeof(client.sa_family)) < 0)
	{
		perror("Bind error");
		return EXIT_FAILURE;
	}

    struct sockaddr server;
    server.sa_family = AF_UNIX;
    strcpy(server.sa_data, "mysocket.s");
	socklen_t serverlen;

	if (sendto(sockfd, buf, strlen(buf) + 1, 0, &server, sizeof(server)) < 0)
	{
		perror("sendto failed");
		return EXIT_FAILURE;
	}

	printf("Client SEND message: %d\n", getpid());

	int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, &server, &serverlen);

	if (bytes < 0)
	{
		perror("recvfrom error");
		return EXIT_FAILURE;
	}

	printf("Client RECIEVED message from server: %s \n", buf);

	close(sockfd);
	unlink(buf);

	return EXIT_SUCCESS;
}