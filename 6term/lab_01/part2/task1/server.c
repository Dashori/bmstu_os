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

	struct sockaddr server;
	server.sa_family = AF_UNIX;
	strcpy(server.sa_data, "mysocket.s");

	if (bind(sockfd, &server, sizeof(server)) < 0)
	{
		perror("Bind error");
		return EXIT_FAILURE;
	}

	struct sockaddr client; 
	socklen_t clientlen;

	char buf[BUF_SIZE];
	while(1)
	{
		int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, &client, &clientlen);

		if (bytes < 0)
		{
			perror("recvfrom error");
			return EXIT_FAILURE;
		}

		printf("Server RECIEVED: %s \n", buf);
	}

	return EXIT_SUCCESS;
}