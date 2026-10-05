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
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1){
		perror("socket");
		return 1;
	}
	printf("Socket created");

	if(close(server_fd)<0){
		perror("Close failed");
		return 1;}
	else{
		printf("\nClose successful");
	}

	return 0;
}
