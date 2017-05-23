#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

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
int *mem;
int notend = 1;

void sigint_hdl(int signum) {
	if(signum == SIGINT)
		notend = 0;
		//exit(0);
}

void vaccuming(void) {
	CHECK(shmdt(mem), -1);
	return;
}

int main(int argc, char **argv)
{
	signal(SIGINT, sigint_hdl);
	srand(time(NULL));
	if (argc != 3) {
		printf("Zła ilośc argumentów");
		exit(0);
	}
	
	key_t key = ftok(argv[1], argv[2][0]);
	CHECK(key, -1);
	
	shared = shmget(key, (BUF_SIZE+1)*sizeof(int), 0600);
	CHECK(shared, -1);
	
	mem = shmat(shared, NULL, 0);
	CHECK((int)mem, -1);
	
	int *i = mem + BUF_SIZE*sizeof(int);
	
	sem = semget(key, 4, 0600); //Jeden dla pisarzy, drugi dla czytelnikow, trzeci (ilosc zadan)
	CHECK(sem, -1);
	
	atexit(vaccuming);
	
	struct sembuf *ops = calloc(3, sizeof(struct sembuf));
	int jobs;
	
	struct timeval czas;
	int retval;
	
	while(notend) {
		ops[0].sem_num = 0;
		ops[0].sem_op = 0;
		ops[1].sem_num = 0;
		ops[1].sem_op = 1;
		do {
			retval = semop(sem, ops, 2);
		}
		while (errno == EINTR && retval == -1);
		CHECK(retval, -1); //Ustawiamy semafor zapisu (czytelnicy nie wchodza)
		
		ops[0].sem_num = 1;
		ops[0].sem_op = 0;
		do {
			retval = semop(sem, ops, 1);
		}
		while (errno == EINTR && retval == -1);
		CHECK(retval, -1); //Czakamy, az czytelnicy skoncza robote
		//Tutaj nikt nic nie rusza procz mnie
	
		//Sprawdzamy czy możemy wpisac cokolwiek do tablicy
		jobs = semctl(sem, 2, GETVAL);
		CHECK(jobs, -1);
		
		gettimeofday(&czas, NULL);
		if(jobs < BUF_SIZE) {
			mem[*i] = rand();
			printf("(%u %lu.%lu) Dodałem liczbę: %u na pozycję %u. Liczba zadań oczekujących: %u.\n", getpid(), czas.tv_sec, czas.tv_usec, mem[*i], *i, jobs+1 );
			*i = (*i+1)%BUF_SIZE;
			
			ops[0].sem_num = 2;
			ops[0].sem_op = 1;
			do {
				retval = semop(sem, ops, 1);
			}
			while (errno == EINTR && retval == -1);
			CHECK(retval, -1); //Podbijamy ilosc robot o 1
		}
		
		ops[0].sem_num = 0;
		ops[0].sem_op = -1;
		do {
			retval = semop(sem, ops, 1);
		}
		while (errno == EINTR && retval == -1);
		CHECK(retval, -1); //Zerujemy semafor zapisu
	}
	exit(0);
}
