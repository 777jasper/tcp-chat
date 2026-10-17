#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8080 // port the server listens on

// struct to hold client information
typedef struct client{
	int sockfd; // this is client socket
	struct sockaddr_in addr; // this is client address
} client_t;

// handle client
void *handle_client(void *arg){
	client_t *c = (client_t *)arg;
	char buf[1024];
	ssize_t n;
	while(1){
		n = recv(c->sockfd, buf, sizeof(buf)-1, 0);

		if(n==-1){
			perror("recv:failed");
			break;
		}
		if(n==0){
			printf("Client disconnected\n");
			break;
		}
		else{
			buf[n] = '\0';
			printf("Received: %s", buf);
			if(send(c->sockfd, buf, n, 0)==-1){
				perror("Send failed");
				break;
			}
		}
	}
	close(c->sockfd); // closing client socket
	printf("Client socket closed\n");

	// Free the dynamically allocated client structure
	free(c);
	
	return NULL;

}

int main(void){
	int server_fd;
	int opt = 1;
	struct sockaddr_in server_addr;



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

	while(1){
	
		client_t *c = malloc(sizeof(client_t));
		if(c == NULL){
			perror("malloc");
			continue;
		}
		socklen_t len = sizeof(c->addr);

		c->sockfd = accept(server_fd, (struct sockaddr *)&c->addr, &len);
		if(c->sockfd == -1){
			perror("accept:failed");
			free(c);
			continue;
		}

		// print which client connected to the server
		/* inet_ntoa: binary IP -> text. ntohs: network byte order -> normal number. */
		
		printf("Client connected: %s:%d\n", inet_ntoa(c->addr.sin_addr), ntohs(c->addr.sin_port));


		pthread_t tid;
		if(pthread_create(&tid, NULL, handle_client, c) != 0){
			printf("Failed to create thread for client \n");
			close(c->sockfd); /* close client socket */
			free(c); /* free the structure assigned using malloc */
			continue;
		}

		pthread_detach(tid);
	}

	return 0;
}
