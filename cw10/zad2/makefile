CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: server client

server: server.c
	$(CC) $< -o $@ $(CFLAGS) -g
	
client: client.c
	$(CC) $< -o $@ $(CFLAGS) -pthread
	
clean:
	rm server 
	rm client
