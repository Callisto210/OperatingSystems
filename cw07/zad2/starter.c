#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>

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
char *name;
char *name2;
sem_t *sem[2];

void vaccuming(void) {

	CHECK(sem_unlink(name), -1);
	CHECK(sem_unlink(name2), -1);
	CHECK(shm_unlink(name), -1);	
	return;
}

void sigint_hdl(int signum) {
	if(signum == SIGINT)
		exit(0);
}

int main(void)
{
	signal(SIGINT, sigint_hdl);
	name = "/alamakota";
	name2 = "/ulamakota";
	shm = shm_open(name, O_CREAT | O_RDWR, 0600);
	CHECK(shm, -1);
	
	CHECK(ftruncate(shm, BUF_SIZE*sizeof(int)), -1);
	
	sem[0] = sem_open(name, O_CREAT, 0600, 0);
	CHECK((int)sem[0], -1);
	
	CHECK(sem_post(sem[0]), -1);
	
	sem[1] = sem_open(name2, O_CREAT, 0600, 0);
	CHECK((int)sem[1], -1);
	
	CHECK(sem_close(sem[0]), -1);
	CHECK(sem_close(sem[1]), -1);
	
	atexit(vaccuming);
	
	while(1)
		pause();
	
	return 0;
}

