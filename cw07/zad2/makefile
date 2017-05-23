CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: server client starter

server: server.c
	$(CC) $< -o $@ $(CFLAGS) -D_GNU_SOURCE -lrt -lpthread -g
	
client: client.c
	$(CC) $< -o $@ $(CFLAGS) -lrt -lpthread -g
	
starter: starter.c
	$(CC) $< -o $@ $(CFLAGS) -lrt -lpthread -g
	
clean:
	rm server 
	rm client
	rm starter
