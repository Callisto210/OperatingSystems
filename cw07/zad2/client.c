#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <semaphore.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

#define BUF_SIZE 10

int shm;
int *mem;
char *name;
char *name2;
sem_t *sem[2];
int notend = 1;

void vaccuming(void) {
	CHECK(sem_close(sem[0]), -1);
	CHECK(sem_close(sem[1]), -1);
	CHECK(munmap(mem, BUF_SIZE), -1);
	return;
}

void sigint_hdl(int signum) {
	if(signum == SIGINT)
		notend = 0;
}

int main(int argc, char **argv)
{
	signal(SIGINT, sigint_hdl);
	int raport = 0;
	if (argc < 2) {
		printf("Zbyt malo argumentow\n");
		exit(1);
	}
		
	if (argc > 2) {
		if (argc == 3 && strcmp(argv[2], "-u") == 0) {
			raport = 1;
		}
		else {
			printf("Niewlasciwe polecenie \n");
			exit(1);
		}
	}
			
	
	char *p = argv[1];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 1 powinien byc liczba\n");
			exit(1);
		}
		p++;
	}
	
	int x = atoi(argv[1]);
	
	name = "/alamakota";
	name2 = "/ulamakota";
	shm = shm_open(name, O_RDONLY, 0600);
	CHECK(shm, -1);
	
	mem = mmap(NULL, BUF_SIZE*sizeof(int), PROT_READ, MAP_SHARED, shm, 0);
	CHECK(mem, MAP_FAILED);
	
	sem[0] = sem_open(name, 0);
	CHECK((int)sem[0], -1);
	
	sem[1] = sem_open(name2, 0);
	CHECK((int)sem[1], -1);
	
	atexit(vaccuming);
	
	struct timeval czas;
	int cntr=0;
	
	while(notend) {
		sem_wait(sem[0]); //Czekamy na wyjscie pisarza (zabieramy mu 1)
		sem_post(sem[1]); //Podbijamy ilosc czytelnikow
		sem_post(sem[0]); //I natychmiast oddajemy
		for (int i=0; i<BUF_SIZE; i++)
			if(mem[i] == x)
				cntr++;
				
		gettimeofday(&czas, NULL);
		if(raport == 0)
			printf("(%u %lu.%lu) Znalazlem %u liczb o wartosci %u\n", getpid(), czas.tv_sec, czas.tv_usec, cntr, x);
		sem_wait(sem[1]); //Obnizamy ilość czytelnikow
	}
	gettimeofday(&czas, NULL);
	if(raport == 1)
		printf("(%u %lu.%lu) Znalazlem %u liczb o wartosci %u\n", getpid(), czas.tv_sec, czas.tv_usec, cntr, x);
	
	return 0;
}

