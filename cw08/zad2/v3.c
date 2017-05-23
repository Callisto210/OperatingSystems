#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
//ala

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

int rows;
int fd;
int threads;
char *tofind;

int num =0;

pthread_t *th;
pthread_mutex_t read_m = PTHREAD_MUTEX_INITIALIZER;

void sighdl (int signum) {
	printf("PID: %d TID: %lu\n", getpid(), pthread_self());
}


void *finder(void *arg) {
	//signal(SIGUSR1, sighdl);
//	sigset_t waitinfor;
//	sigemptyset(&waitinfor);
//	sigaddset(&waitinfor, SIGUSR1);
//	CHECK_DIFF(pthread_sigmask(SIG_SETMASK, &waitinfor, NULL), 0);
//int ala;
//printf("%d\n", num);
//if (num == 2) {
//	ala = 1/0;
//}
//num++;
	
	CHECK_DIFF(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL), 0);
	char **buff = calloc(sizeof(char*), rows);
	for (int i=0; i<rows; i++)
		buff[i] = calloc(sizeof(char), 1024);

	int read_more = 1;
	while (read_more) {
		sleep(1);
		CHECK_DIFF(pthread_mutex_lock(&read_m), 0);
		
		int rows_readed;
		for (rows_readed=0; rows_readed<rows; rows_readed++) {
			read_more = read(fd, buff[rows_readed], 1024);
			CHECK(read_more, -1);
			if (read_more == 0)
				break;
			buff[rows_readed][read_more] = '\0';
		}

		CHECK_DIFF(pthread_mutex_unlock(&read_m), 0);
		for (int i=0; i<rows_readed; i++) {
			if(strstr(buff[i]+5, tofind) != NULL)
				printf("\n%lu, Znaleziono szukany string w: %s \n", pthread_self(), buff[i]);
		}
	}
	pthread_exit(0);
}

int main(int argc, char **argv)
{
	if (argc < 5) {
		printf("Zbyt malo argumentow\n");
		exit(1);
	}

	//ilosc watkow
	char *p = argv[1];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 1 powinien byc liczba\n");
			exit(1);
		}
		p++;
	}
	
	//ilosc rekordow czytanych przez watek
	char *q = argv[3];
	while (*q != 0) {
		if (isdigit((int)*q) == 0) {
			printf("Argument 3 powinien byc liczba\n");
			exit(1);
		}
		q++;
	}
	
	tofind = argv[4];
	threads = atoi(argv[1]);
	rows = atoi(argv[3]);
	fd = open(argv[2], O_RDONLY);
	
	CHECK(fd, -1);
	CHECK_DIFF(pthread_mutex_init(&read_m, NULL), 0);
	
	th = malloc(threads*sizeof(pthread_t));
	
	printf("MAIN PROCESS PID: %d TID: %lu\n", getpid(), pthread_self());

	for (int i=0; i<threads; i++)
		CHECK_DIFF(pthread_create(&th[i], NULL, finder, NULL), 0);
		
	sigset_t waitinfor;
	sigemptyset(&waitinfor);
	sigaddset(&waitinfor, SIGUSR1);
	CHECK_DIFF(pthread_sigmask(SIG_BLOCK, &waitinfor, NULL), 0);

	sleep(2);
	//kill(getpid(), SIGUSR1);
	CHECK_DIFF(pthread_kill(th[0], SIGUSR1), 0);	
	
	for (int i=0; i<threads; i++)
		CHECK_DIFF(pthread_join(th[i], NULL), 0);
		
	CHECK_DIFF(pthread_mutex_destroy(&read_m), 0);
	CHECK(close(fd), -1);
	return 0;
}
//ala
