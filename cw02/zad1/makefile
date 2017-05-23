CC=gcc
CFLAGS=-Wall -pedantic

default: generator sort

generator: generator.o
	$(CC) $< -o $@
	
sort: sort.o
	$(CC) $< -o $@

generator.o: generator.c
	$(CC) $< -c -o $@ $(CFLAGS)

sort.o: sort.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm generator
	rm sort
	rm *.o
	
