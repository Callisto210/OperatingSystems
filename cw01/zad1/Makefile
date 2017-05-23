CC=gcc
LIB=./lib
SRC=./src

default: liblists.a liblist.so

liblists.a: list_static.o
	ar rcsv $(LIB)/liblists.a list_static.o
	
liblist.so: list_shared.o
	$(CC) -shared -Wl,-soname,liblist.so -o $(LIB)/liblist.so list_shared.o
	
list_static.o:
	$(CC) -c -Wall -pedantic $(SRC)/list.c -o list_static.o
	
list_shared.o:
	$(CC) -fPIC -c -Wall -pedantic $(SRC)/list.c -o list_shared.o
	
clean:
	rm ./*.o
	rm ./lib/*
	
