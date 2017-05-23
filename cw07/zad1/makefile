CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: server client starter

server: server.c
	$(CC) $< -o $@ $(CFLAGS) -D_GNU_SOURCE
	
client: client.c
	$(CC) $< -o $@ $(CFLAGS)
	
starter: starter.c
	$(CC) $< -o $@ $(CFLAGS)
	
clean:
	rm server 
	rm client
	rm starter
