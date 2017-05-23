CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: sender catcher

sender: sender.c
	$(CC) $< -o $@ $(CFLAGS)
	
catcher: catcher.c
	$(CC) $< -o $@ $(CFLAGS)
	
clean:
	rm catcher
	rm sender
