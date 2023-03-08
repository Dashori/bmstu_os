#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

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
   	char buf[14];
    sprintf(buf, "client: %d", getpid()); // 14
    strcpy(client.sa_data, buf);

	struct sockaddr server;
    server.sa_family = AF_UNIX;
    strcpy(server.sa_data, "mysocket.s");

	if (sendto(sockfd, buf, strlen(buf) + 1, 0, &server, sizeof(server)) < 0)
	{
		perror("sendto failed");
		return EXIT_FAILURE;
	}

	printf("Client SEND message: %d", getpid());

	close(sockfd);
	unlink(buf);

	return EXIT_SUCCESS;
}
