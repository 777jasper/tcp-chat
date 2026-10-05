CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -pthread

all: chat_server chat_client

chat_server: server.c
	$(CC) $(CFLAGS) -o chat_server server.c

chat_client: client.c
	$(CC) $(CFLAGS) -o chat_client client.c

clean:
	rm -f chat_server chat_client

.PHONY: all clean
