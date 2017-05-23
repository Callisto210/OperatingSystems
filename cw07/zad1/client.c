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
int *mem;
int sem;
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
	
	shared = shmget(key, BUF_SIZE*sizeof(int), 0600);
	CHECK(shared, -1);
	
	mem = shmat(shared, NULL, SHM_RDONLY);
	CHECK((int)mem, -1);
	
	int i;
	int *j = mem + BUF_SIZE*sizeof(int);

	sem = semget(key, 4, 0600); //Jeden dla piszarzy, drugi dla czytelnikow, trzeci (ilosc zadan), czwarty - wewnetrzny dla czytelnikow
	CHECK(sem, -1);
	
	atexit(vaccuming);
	
	struct sembuf *ops = calloc(5, sizeof(struct sembuf));
	int jobs;
	
	struct timeval czas;
	int retval;
	
	while (notend) {
		ops[0].sem_num = 2;
		ops[0].sem_op = -1;//Zmniejszamy ilosc roboty, przy okazji blokuje nas na niezerowa jej ilosc
		ops[1].sem_num = 1;
		ops[1].sem_op = 1;//Powiekszamy liczbe czytelnikow
		ops[2].sem_num = 3;
		ops[2].sem_op = 0;//Czekamy az bedziemy mieli exclusive do tego frag z jobs
		ops[3].sem_num = 3;
		ops[3].sem_op = 1;//Jak bedziemy mieli to ustawimy jedynke
		ops[4].sem_num = 0;
		ops[4].sem_op = 0;//sprawdzamy czy nie ma pisarza
		do {
			retval = semop(sem, ops, 5);
		}
		while (errno == EINTR && retval == -1);
		CHECK(retval, -1);
		
		jobs = semctl(sem, 2, GETVAL);
		CHECK(jobs, -1);
		
		i = *j - jobs - 1;
		if (i<0)
			i+=BUF_SIZE;
		
		//Pod zmienną 'i' mamy miejsce w tablicy z którego będziemy czytac
		ops[0].sem_num = 3;
		ops[0].sem_op = -1;
		do {
			retval = semop(sem, ops, 1);
		}
		while (errno == EINTR && retval == -1); //Pozwalamy tym wyżej zejsc nizej
		CHECK(retval, -1);
		
		gettimeofday(&czas, NULL);
		if (mem[i]%2 == 0)
			printf("(%u %lu.%lu) Sprawdziłem liczbę %u na pozycji %u - parzysta. Pozostało zadań oczekujących: %u\n", getpid(), czas.tv_sec, czas.tv_usec, mem[i], i, jobs+1 );
		else
			printf("(%u %lu.%lu) sprawdziłem liczbę %u na pozycji %u - nieparzysta. Pozostało zadań oczekujących: %u\n", getpid(), czas.tv_sec, czas.tv_usec, mem[i], i, jobs+1 );

		ops[0].sem_num = 1;
		ops[0].sem_op = -1;
		do {
			retval = semop(sem, ops, 1);
		}
		while (errno == EINTR && retval == -1);
		CHECK(retval, -1); //Wychodzimy czyli zmniejszamy ilość czytelnikow
	}	
	exit(0);
}

