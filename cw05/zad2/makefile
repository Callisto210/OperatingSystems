CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: rdfifo wrfifo

rdfifo: rdfifo.c
	$(CC) $< -o $@ $(CFLAGS)
	
wrfifo: wrfifo.c
	$(CC) $< -o $@ $(CFLAGS)
	
clean:
	rm rdfifo 
	rm wrfifo
