#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

mqd_t queue;

void vaccuming(void) {
	CHECK(mq_close(queue), -1);
	return;
}

int main (int argc, char **argv) {
	srand(time(NULL));
	if (argc != 2) {
		printf("Zła ilośc argumentów");
		exit(0);
	}
	
	int len = strlen(argv[1]);
	if (len > NAME_MAX) {
		printf("String zbyt dlugi\n");
		exit(1);
	}
	if (argv[1][0] != '/') {
		printf("String sie zle zaczyna\n");
		exit(1);
	}
	
	int client_cntr = 0;
	mqd_t client[128];
	char recv[8192];
	int pri;
	char id;
	
	queue = mq_open(argv[1], O_RDONLY | O_CREAT, 0600, NULL);
	CHECK((int)queue, -1);
	atexit(vaccuming);
	
	struct mq_attr attr;
	CHECK(mq_getattr(queue, &attr), -1);
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 256;
	CHECK(mq_setattr(queue, &attr, NULL), -1);
	
	unsigned int priority;
	
	while(1) {
		CHECK(mq_receive(queue, recv, 8192, &priority), -1);
		switch (priority) {
			case 1: //Klient sie zglosil
			client[client_cntr] = mq_open(recv, O_WRONLY);
			CHECK((int)client[client_cntr], -1);
			
			//Odbijamy klientowi jego numer (mtype = 1)
			CHECK(mq_send(client[client_cntr], (char *)&client_cntr, sizeof(int), 1), -1);
			client_cntr++;
			break;
			
			case 2: //Klient zglasza gotowosc, wysylamy prace
			id = recv[0];
			pri = rand();
			CHECK(mq_send(client[(int)id], (char *)&pri, sizeof(int), 1), -1);
			break;
			
			case 3: //Odbieramy robote
			id = recv[0];
			pri = *(int *)(recv+1);
			char isprime = recv[1+sizeof(int)];
			if(isprime)
				printf("Liczba pierwsza: %u (klient: %u)\n", pri, id);
			break;
			
			case 5: //Klient kaze zamknac z tej strony
			id = recv[0];
			CHECK(mq_close(client[(int)id]), -1);
			break;
		}
	}
	return 0;
}
