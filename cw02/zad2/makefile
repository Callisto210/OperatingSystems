CC=gcc
CFLAGS=-Wall -pedantic

default: katalogi katalogi_nftw

katalogi: katalogi.o
	$(CC) $< -o $@
	
katalogi_nftw: katalogi_nftw.o
	$(CC) $< -o $@

katalogi.o: katalogi.c
	$(CC) $< -c -o $@ $(CFLAGS)

katalogi_nftw.o: katalogi_nftw.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm katalogi
	rm katalogi_nftw
	rm *.o
	
