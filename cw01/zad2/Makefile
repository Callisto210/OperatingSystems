CC=gcc
SRC=./src

default: listy_static listy_shared listy_dll test
	
listy_static: listy.o
	$(CC) -o listy_static listy.o -L ./../zad1/lib -llists
	
listy_shared: listy.o
	$(CC) -o listy_shared listy.o -L ./../zad1/lib -llist -Wl,-rpath,./../zad1/lib

listy_dll:
	$(CC) -o listy_dll $(SRC)/listydll_test.c -Wall -ldl -I ./../zad1/include
	
listy.o:
	$(CC) -c $(SRC)/listy_test.c -o listy.o -Wall -I ./../zad1/include

test: test_static test_shared test_dll

test_static:
	./listy_static --test > static_output
	diff correct static_output
	
test_shared:
	./listy_shared --test > shared_output
	diff correct shared_output
	
test_dll:
	./listy_dll --test > dll_output
	diff correct dll_output

clean:
	rm *output
	rm *.o
	rm listy_static
	rm listy_shared
	rm listy_dll
