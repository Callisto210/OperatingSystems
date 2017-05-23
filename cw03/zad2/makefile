CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: fcounter

fcounter: fcounter.c
	$(CC) $< -o $@ $(CFLAGS)
	
clean:
	rm fcounter
