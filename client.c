#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>

#define PORT 8080	//must match the port the server listens on
#define SERVER_IP "127.0.0.1"	// loopback address = this same computer

// read data from server and prints it
void *receive_loop(void *arg){
	int sock_fd = *(int *)arg;
	char buf[1024];
	ssize_t n;

	while(1){
		n = recv(sock_fd, buf, sizeof(buf)-1,0);

		if(n==-1){
			perror("recv: failed");
			break;
		}
		if(n==0){
			printf("Server closed the connection\n");
			break;
		}
		else{
			buf[n] = '\0';
			printf("%s",buf);
		}
	}

	//closing the socket
	if(close(sock_fd)<0){
		perror("Client socket close failed");
		exit(1);
	}
	printf("Client socket closed\n");
	exit(0);
	return NULL;
}






int main(void)
{
    int sock_fd;
    struct sockaddr_in server_addr;
    char buf[1024];
    
    /* Create a TCP socket: AF_INET = IPv4, SOCK_STREAM = TCP.
     * Returns a file descriptor, or -1 on failure. */

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd == -1){
	    perror("Client socket creation failed");
	    return 1;
    }
    printf("Client socket created\n");

    /* Build the server's address.
     * memset clears the struct so unused fields don't hold garbage. */

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    /* inet_pton: convert the text IP ("127.0.0.1") into binary form.
     * Returns 1 on success; anything else means the address is invalid. */

    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1){
	    printf("Invalid server address\n");
	    close(sock_fd);
	    return 1;
    }

    /* connect: start the TCP connection to the server.
     * Fails with "Connection refused" if no server is listening there.
     * The client needs no bind() or listen(); the system picks its port. */

    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
	    perror("No server listening");
	    close(sock_fd);
	    return 1;
    }

    printf("Connected to the server\n");

    pthread_t tid;
    if(pthread_create(&tid, NULL, receive_loop, &sock_fd) !=0){
	    perror("Thread creation failed");
	    close(sock_fd);
	    return 1;
    }
    pthread_detach(tid);




    while(1){
	    // read one line from the keyboard (include new line)

	    if(fgets(buf, sizeof(buf), stdin) == NULL){
		    break;	// Ctrl+D: end of input
	    }

	    // optional: type /quit to leave
	    if(strncmp(buf, "/quit", 5)==0)
		    break;
	    // send only the typed characters, not the whole buffer
	    if(send(sock_fd, buf, strlen(buf), 0) == -1){
		    perror("send: failed");
		    break;
	    }
    }


    /* Cleanup: close the socket so the file descriptor is released
     * and the server sees that we disconnected. */

    if(close(sock_fd)<0){
	    perror("Client socket close failed");
	    return 1;
    }
    else{
	    printf("Client socket closed\n");
    }


    return 0;
}
