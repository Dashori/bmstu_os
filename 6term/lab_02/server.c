#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

#define BUF_SIZE 50
#define CLIENT_NUM 5

int sockfd;

void signal_handler(int signal)
{
    printf("\nCaught signal = %d\n", signal);
    close(sockfd);
    printf("\nServer exiting.\n");
    exit(0);
}

int main(int argc, char** argv) 
{
	if (argc != 2)
    {
        perror("args not enough\n");
        return EXIT_FAILURE;
    }

    short port = (short) atoi(argv[1]);

	if ((signal(SIGINT, signal_handler) == SIG_ERR)) {
        perror("Can't attach handler\n");
        return EXIT_FAILURE;
    }

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) 
	{
		perror("socket() failed");
        return EXIT_FAILURE;
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*) &sa, sizeof(sa)) < 0) 
	{
		perror("bind() failed");
		return EXIT_FAILURE;
	}

	listen(sockfd, CLIENT_NUM);

	char buf[BUF_SIZE];
	int k = 0;
	struct pollfd pfd[CLIENT_NUM + 1];
    pfd[0].fd = sockfd;
    pfd[0].events = POLLIN;   // Есть данные для чтения.

	while (1)
	{
		int rc = poll(pfd, k + 1, 10000);
        if (rc == -1)
        {
            perror("poll() failed");
            return EXIT_FAILURE;
        }
        else if (rc > 0)
        {
        	if ((pfd[0].revents == 1) & POLLIN) 
			{
				struct sockaddr_in sa;
				int addrSize = sizeof(sa);

				pfd[0].revents = 0;
	            pfd[++k].fd = accept(sockfd, (struct sockaddr*) &sa, (socklen_t*) &addrSize);
				if (pfd[k].fd < 0)
				{
				    perror("accept() failed");
				    k--;
				    return EXIT_FAILURE;
				}
				pfd[k].events = POLLIN;
				    
				printf("\nClient connected №%d %s:%d\n", 
	                        k, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
			} 
			else
			{
				for (int i = 1; i < k + 1; i++)
				{
					if ((pfd[i].revents  == 1) & POLLIN)
					{
						pfd[i].revents = 0;
						if (read(pfd[i].fd, buf, sizeof(buf)) == 0)
	                    {
	                        printf("\nclient %d disconnected\n", i);
	                        pfd[i].fd = 0;
	                    }
	                    else
	                    {
	                    	printf("\nreceived message: %s\n", buf);
							sprintf(buf, "%s %d", buf, getpid());
							write(pfd[i].fd, buf, sizeof(buf));
							printf("\nsent message: %s\n", buf);
	                    }
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}