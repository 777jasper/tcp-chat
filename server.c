#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8080

int main(void){
	int server_fd;
	int opt = 1;
	struct sockaddr_in server_addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1){
		perror("socket");
		return 1;
	}
	printf("Socket created\n");

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		perror("setsockopt");
		close(server_fd);
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	printf("Address configured\n");

	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("bind");
		close(server_fd);
		return 1;
	}

	if(listen(server_fd, 10) == -1){
		perror("listen");
		close(server_fd);
		return 1;
	}
	printf("Listening on port %d\n",PORT);


	if(close(server_fd)<0){
		perror("Close failed");
		return 1;}
	else{
		printf("Close successful");
	}

	return 0;
}
