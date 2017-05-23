CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: server client

server: server.c
	$(CC) $< -o $@ $(CFLAGS) -D_GNU_SOURCE -g
	
client: client.c
	$(CC) $< -o $@ $(CFLAGS) -lm
	
clean:
	rm server 
	rm client
