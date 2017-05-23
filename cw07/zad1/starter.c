#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 10

#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

int shared;
int sem;

void sigint_hdl(int signum) {
	if(signum == SIGINT)
		exit(0);
}

void vaccuming(void) {
	CHECK(shmctl(shared, IPC_RMID, NULL), -1);
	CHECK(semctl(sem, -1, IPC_RMID), -1);
	return;
}

int main(int argc, char **argv)
{
	signal(SIGINT, sigint_hdl);
	if (argc != 3) {
		printf("Zła ilośc argumentów");
		exit(0);
	}
	
	key_t key = ftok(argv[1], argv[2][0]);
	CHECK(key, -1);
	
	shared = shmget(key, (BUF_SIZE+1)*sizeof(int), IPC_CREAT | 0600);
	CHECK(shared, -1);
	
	sem = semget(key, 4, IPC_CREAT | 0600); //Jeden dla pisarzy, drugi dla czytelnikow, trzeci (ilosc zadan)
	CHECK(sem, -1);
	
	atexit(vaccuming);
	
	while(1)
		pause();
	
	return 0;
}

