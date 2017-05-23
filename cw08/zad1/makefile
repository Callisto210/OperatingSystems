CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: generator v1 v2 v3

generator: generator.c
	$(CC) $< -o $@ $(CFLAGS)
	
v1: v1.c
	$(CC) $< -o $@ $(CFLAGS) -pthread
	
v2: v2.c
	$(CC) $< -o $@ $(CFLAGS) -pthread -g
	
v3: v3.c
	$(CC) $< -o $@ $(CFLAGS) -pthread
	
clean:
	rm v1 
	rm v2
	rm v3
	rm generator
