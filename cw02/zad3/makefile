CC=gcc
CFLAGS=-Wall -pedantic

default: rygiel

rygiel: rygiel.o
	$(CC) $< -o $@

rygiel.o: rygiel.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm rygiel
	rm rygiel.o
	
