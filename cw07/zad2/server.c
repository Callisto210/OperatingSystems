#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>

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

int main(void)
{
	signal(SIGINT, sigint_hdl);
	srand(time(NULL));
	name = "/alamakota";
	name2 = "/ulamakota";
	shm = shm_open(name, O_RDWR, 0600);
	CHECK(shm, -1);
		
	mem = mmap(NULL, BUF_SIZE*sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
	CHECK(mem, MAP_FAILED);
	
	sem[0] = sem_open(name, 0);
	CHECK((int)sem[0], -1);
		
	sem[1] = sem_open(name2, 0);
	CHECK((int)sem[1], -1);
	
	atexit(vaccuming);
	int sem_val;
	int first = rand()%50;
	int pos;
	struct timeval czas;

	
	while(notend) {
		sem_wait(sem[0]); //Jeśli możemy wejsc to wchodzimy
			
		do {
			CHECK(sem_getvalue(sem[1], &sem_val), -1);
		}
		while (sem_val != 0); //Czekamy aż wyjda czytelnicy
		
		for (int i=rand()%BUF_SIZE; i>=0; i--) {
			gettimeofday(&czas, NULL);
			pos = rand()%BUF_SIZE;
			mem[pos] = first++;
			first %= 50;
			printf("(%u %lu.%lu) Wpisałem liczbę %u - na pozycję %u. Pozostało %u zadań\n", getpid(), czas.tv_sec, czas.tv_usec, mem[pos], pos, i );
		}
		sem_post(sem[0]);
	}
	
	return 0;
}

