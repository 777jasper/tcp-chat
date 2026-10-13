#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080	//must match the port the server listens on
#define SERVER_IP "127.0.0.1"	// loopback address = this same computer

int main(void)
{
    int sock_fd;
    struct sockaddr_in server_addr;

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
	    perror("Connection refused");
	    close(sock_fd);
	    return 1;
    }

    printf("Connected to the server\n");

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
