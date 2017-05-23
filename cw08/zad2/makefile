CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: v3

v3: v3.c
	$(CC) $< -o $@ $(CFLAGS) -pthread
	
clean:
	rm v3
