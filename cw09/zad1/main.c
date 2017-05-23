#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)
    
#define CHECK_DIFF(f, r)                       				     \
    do {                                                         \
        if (f != r)               				                 \
        {                                                        \
            printf(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

sem_t sem[5];

void *philosopher(void *arg) {
	int id = (int)arg;
	printf("ID %d \n", id);
	while(1) {
		if(id != 0) {
			CHECK(sem_wait(sem+id), -1);
			CHECK(sem_wait(sem+((id+1)%5)), -1);
			printf("AMMMM... ID %d\n", id);
			CHECK(sem_post(sem+((id+1)%5)), -1);	
			CHECK(sem_post(sem+id), -1);
		}
		else {
			CHECK(sem_wait(sem+(id+1)%5), -1);
			CHECK(sem_wait(sem+id), -1);
			printf("AMMMM... ID %d\n", id);
			CHECK(sem_post(sem+id), -1);
			CHECK(sem_post(sem+(id+1)%5), -1);	
		}
	}
	pthread_exit(0);
}

int main(void)
{
	pthread_t filo[5];
	
	for (int i=0; i<5; i++)
		CHECK(sem_init(sem+i, 0, 1), -1); //Drugi argument 0 -> wspoldzielimy miedzy watkami
	
	for (int i=0; i<5; i++)
		CHECK_DIFF(pthread_create(filo+i, NULL, philosopher, (void *)i), 0);
	
	for (int i=0; i<5; i++)
		CHECK_DIFF(pthread_join(filo[i], NULL), 0);
		
	for (int i=0; i<5; i++)
		CHECK(sem_destroy(sem+i), -1);
	
	return 0;
}

