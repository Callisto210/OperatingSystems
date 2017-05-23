#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define PLANES 20
#define CAP 8
#define K 4

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

pthread_mutex_t ilosc_m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t start = PTHREAD_COND_INITIALIZER;
pthread_cond_t land = PTHREAD_COND_INITIALIZER;

int ilosc;
int ile_startuje;
int ile_laduje;

typedef enum {
	STARTUJE,
	LECI,
	LADUJE,
	STOI
} state;

void *plane(void *arg) {
	int l = (int)arg;
	state stan = (l<K) ? STOI : LECI;
	
	while(1) {
		switch (stan) {
			case STARTUJE:
				pthread_mutex_lock(&m);
				do {
					pthread_mutex_lock(&ilosc_m);
					if (ilosc > 0) {
						ilosc--;
						stan = LECI;
						printf("Samolot nr %d startuje\n", l);
						if (ilosc <= K && ile_laduje > 0) {
							pthread_cond_signal(&land);
							ile_laduje--;
						}
						else if (ile_startuje > 0) {
							pthread_cond_signal(&start);
							ile_startuje--;
						}
						pthread_mutex_unlock(&ilosc_m);
						break;
					}
					else {
						ile_startuje++;
						pthread_mutex_unlock(&ilosc_m);
						pthread_cond_wait(&start, &m);
					}
				} while(1);
				pthread_mutex_unlock(&m);
				break;
		
			case LECI:
				usleep(rand()%5);
				printf("Samolot nr %d chce ladowac\n", l);
				stan = LADUJE;
				break;
			
			case LADUJE:
				pthread_mutex_lock(&m);
				do {
					pthread_mutex_lock(&ilosc_m);
					if (ilosc < CAP) {
						ilosc++;
						stan = STOI;
						printf("Samolot nr %d laduje\n", l);
						if (ilosc >= K && ile_startuje > 0) {
							pthread_cond_signal(&start);
							ile_startuje--;
						}
						else if (ile_laduje > 0) {
							pthread_cond_signal(&land);
							ile_laduje--;
						}
						pthread_mutex_unlock(&ilosc_m);
						break;
					}
					else {
						ile_laduje++;
						pthread_mutex_unlock(&ilosc_m);
						pthread_cond_wait(&land, &m);
					}
				} while(1);
				pthread_mutex_unlock(&m);
				break;
			
			case STOI:
				usleep(rand()%2);
				printf("Samolot nr %d chce startowac\n", l);
				stan = STARTUJE;
				break;
		}
	}
	pthread_exit(0);
}

int main(void)
{
	srand(time(NULL));
	pthread_t *p = malloc(sizeof(pthread_t)*PLANES);
	ilosc = 4;
	ile_startuje = 0;
	ile_laduje = 0;
	for(int i=0; i<PLANES; i++)
		CHECK_DIFF(pthread_create(p+i, NULL, plane, (void *)i), 0);
	
	
	
	
		
	for (int i=0; i<PLANES; i++)
		CHECK_DIFF(pthread_join(p[i], NULL), 0);
		
	pthread_mutex_destroy(&m);
	return 0;
}

