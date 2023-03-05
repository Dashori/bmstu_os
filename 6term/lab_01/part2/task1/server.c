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

	struct sockaddr from; 
	socklen_t fromlen;

	struct sockaddr to;
	to.sa_family = AF_UNIX;
	strcpy(to.sa_data, "mysocket.s");

	if (bind(sockfd, &to, strlen(to.sa_data) + 1 + sizeof(to.sa_family)) < 0)
	{
		perror("Bind error");
		return EXIT_FAILURE;
	}

	char buf[BUF_SIZE];
	while(1)
	{
		int bytes = recvfrom(sockfd, buf, sizeof(buf), 0, &from, &fromlen);

		if (bytes < 0)
		{
			perror("recvfrom error");
			close(sockfd);
			unlink("mysocket.s");

			return EXIT_FAILURE;
		}

		printf("Server recieved: %s \n", buf);

		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}