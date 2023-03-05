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
	from.sa_family = AF_UNIX;
    char buf[BUF_SIZE];
    snprintf(buf, BUF_SIZE, "client %d", getpid()); // 14
    strcpy(from.sa_data, buf);

	struct sockaddr to;
    to.sa_family = AF_UNIX;
    strcpy(to.sa_data, "mysocket.s");

	if (sendto(sockfd, buf, strlen(buf) + 1, 0, &to, strlen(to.sa_data) + sizeof(to.sa_family) + 1) < 0)
	{
		perror("sendto failed");
		return EXIT_FAILURE;
	}

	printf("Client send message: %d", getpid());

	close(sockfd);
	unlink(buf);

	return EXIT_SUCCESS;
}