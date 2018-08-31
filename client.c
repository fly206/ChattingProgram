#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#define PORT 8031
#define MASDATASIZE 100

int main(int argc, char *argv[]) {
	int sockfd, num = 1, pid1, pid0, i;
	char buf[MASDATASIZE];
	char killd[10];
	struct sockaddr_in server;
	struct hostent* he;
	memset(killd, 0, sizeof(killd));
	if(argc != 2) {
		printf("Usage: %s <IP Addres>\n", argv[0]);
		exit(1);
	}
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() error");
		exit(1);
	}
	bzero(&server, sizeof(server));
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT);
	inet_aton(argv[1], &(server.sin_addr));
	if((he = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname() error");
		exit(1);
	}
	server.sin_addr = *((struct in_addr*)he->h_addr);
	if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		perror("connect() error");
		exit(1);
	}
	printf("connect succeed\n");
	pid0 = getpid();
	if((pid1 = fork()) < 0) {
		printf("fork1 failed!\n");
		exit(0);
	}
	else if(pid1 == 0) {
		while(getppid() != 1) {
			memset(buf, 0, sizeof(buf));
			printf("Send Message:\n");
			scanf("%[^\n]%*c",buf);
			if(!strlen(buf)) {
				break;
			}
			for(i=0; i<strlen(buf); i++) {
				buf[i] = buf[i] + i;
			}
			send(sockfd, buf, strlen(buf), 0);
			//printf("%d\n",strcmp(buf,"exit"));
			}
			if(getppid() != 1) {
				sprintf(killd,"kill %d",getppid());
				system(killd);
			}
			printf("disconnect\n");
			close(sockfd);
			exit(0);
		}
	while(1) {
		memset(buf, 0, sizeof(buf));
		if((num = recv(sockfd, buf, MASDATASIZE,0)) == -1) {
			perror("recv() error");
			exit(1);
		}
		if(!strlen(buf)) {
			break;
		}
		//system("clear");
		for(i=0; i<strlen(buf); i++){
			buf[i] = buf[i] - i;
		}
		buf[num] = '\0';
		printf("\nReceived Message:\n%s\nSend Message:\n", buf);
	}
	printf("disconnect\n");
	if(kill(pid1,0) == 0) {
		sprintf(killd,"kill %d",pid1);
		system(killd);
	}
	sprintf(killd,"kill %d",pid1);
	system(killd);
	close(sockfd);
	return 0;
}

