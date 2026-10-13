#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8080 // port the server listens on

int main(void){
	int server_fd, client_fd;
	int opt = 1;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	/* Create a TCP socket: AF_INET = IPv4, SOCK_STREAM = TCP.
	 * Returns a file descriptor or -1 on failure. */

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1){
		perror("creating server socket: failed");
		return 1;
	}
	printf("Server socket created\n");

	/* Let the port be reused right after the server stops. Without this, restarting can fail with "Address already in use". */

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		perror("Server socket reuse: failed");
		close(server_fd);
		return 1;
	}


	/* Build the server address. memset clears the struct so unused fields don't hold garbage. */

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	printf("Server Address configured\n");

	/* bind: attach the socket to the address and port above. Fails with "Address already in use" if another program holds the port. */

	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("bind: failed");
		close(server_fd);
		return 1;
	}

	/* listen: mark the socket as a server socket.
	 * 10 = how many waiting connections the system may queue. */

	if(listen(server_fd, 10) == -1){
		perror("listen: failed");
		close(server_fd);
		return 1;
	}
	printf("Listening on port %d\n",PORT);

	/* accept: BLOCKS (waits) until a client connects.
	 *  Returns a NEW socket (client_fd) used only for that client;
	 * server_fd stays open for listening. Also fills in client_addr. */

	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	if(client_fd == -1){
		perror("accept: failed");
		close(server_fd);
		return 1;
	}


	/* inet_ntoa: binary IP -> text. ntohs: network byte order -> normal number. */

	printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	if(close(client_fd)<0){
		perror("Client socket close failed");
		return 1;
	}
	else{
		printf("Client socket close succeful\n");
	}



	if(close(server_fd)<0){
		perror("Server socket close failed");
		return 1;
	}
	else{
		printf("Server socket close successful\n");
	}

	return 0;
}
