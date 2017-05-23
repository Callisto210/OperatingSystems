CC=gcc
CFLAGS=-Wall -Wextra -pedantic

default: fork vfork clone clonev

fork: fork.c
	$(CC) $< -o $@ $(CFLAGS)
	
vfork: vfork.c
	$(CC) $< -o $@ $(CFLAGS)
	
clone: clone.c
	$(CC) $< -o $@ $(CFLAGS)
	
clonev: clonev.c
	$(CC) $< -o $@ $(CFLAGS)
	
clean:
	rm fork
	rm vfork
	rm clone
	rm clonev
