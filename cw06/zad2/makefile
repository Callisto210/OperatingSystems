CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: server client

server: server.c
	$(CC) $< -o $@ $(CFLAGS) -D_GNU_SOURCE -lrt
	
client: client.c
	$(CC) $< -o $@ $(CFLAGS) -lm -lrt
	
clean:
	rm server 
	rm client
