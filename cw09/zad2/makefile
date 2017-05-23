CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: main

main: main.c
	$(CC) $< -o $@ $(CFLAGS) -pthread
	
clean:
	rm main
