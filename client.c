#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main(void)
{
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd == -1){
	    perror("Socket creation failed");
	    return 1;
    }
    printf("Socket created\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);


    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1){
	    printf("Invalid address\n");
	    close(sock_fd);
	    return 1;
    }

    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
	    perror("connect");
	    close(sock_fd);
	    return 1;
    }

    printf("Connected to the server\n");


    if(close(sock_fd)<0){
	    perror("Close failed");
	    return 1;
    }
    else{
	    printf("Close succeful\n");
    }


    return 0;
}
