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
#define BACKLOG 5  					//connection number
#define MASDATASIZE 100

int main(int argc, char* argv[]) {
	int listenfd, connectfd, pid1, i, num = 1;
	int opt = SO_REUSEADDR;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	char buf[MASDATASIZE];
	char killd[10];
	memset(buf, 0, sizeof(buf));
	memset(killd, 0, sizeof(killd));
	if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() error.\n");
		exit(1);
	}
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	memset(&server, 0, sizeof(server));
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		perror("Bind() error\n");
		exit(1);
	}
	if(listen(listenfd, BACKLOG) == -1) {
		perror("listrn() error\n");
		exit(1);
	}
	addrlen = sizeof(client);
	if((connectfd=accept(listenfd, 
		(struct sockaddr*)&client, &addrlen)) == -1) {
		perror("accept() error\n");
		exit(1);
	}
	printf("Client's IP is %s, port is %d \n",
			inet_ntoa(client.sin_addr), htons(client.sin_port));
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
			for(i=0; i<strlen(buf); i++){
				buf[i] = buf[i] + i;
			}
			send(connectfd, buf, strlen(buf), 0);
			//printf("%d\n",strcmp(buf,"exit"));
			}
			if(getppid() != 1) {
				sprintf(killd,"kill %d",getppid());
				system(killd);
			}
			printf("disconnect\n");
			close(connectfd);
			close(listenfd);
			exit(0);
	}
	//else if(pid1 > 0)
		//printf("pid=%d\n",pid1);
	while(1){
		//printf("test child num=%d",num);
		memset(buf, 0, sizeof(buf));
		if((num = recv(connectfd, buf, MASDATASIZE,0)) == -1) {
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
	close(connectfd);
	close(listenfd);
	return 0;
}

