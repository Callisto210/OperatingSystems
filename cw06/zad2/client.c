#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <mqueue.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
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
mqd_t server;
char id;

void vaccuming(void) {
	CHECK(mq_send(server, &id, 1, 5), -1);
	CHECK(mq_close(queue), -1);
	return;
}

char prime(int p) {
	if(p == 0 || p == 1)
		return 0;
	for (int i=2; i<=sqrt(p); i++)
		if(p%i == 0)
			return 0;
	return 1;
}

int main (int argc, char **argv) {
	if (argc != 2) {
		printf("Zla ilosc argumentow");
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
	
	char recv[256];
	char send[256];
	char name[32];
	name[0] = '/';
	name[31] = '\0';
	
	for(int i=1; i<31; i++)
		name[i] = (char)rand()%94+33;// 33-126
	
	server = mq_open(argv[1], O_WRONLY);
	CHECK((int)server, -1);
	
	struct mq_attr attr = {	.mq_maxmsg = 10,
							.mq_msgsize = 255
							};
	
	queue = mq_open(name, O_CREAT | O_RDONLY, 0600, &attr);
	CHECK((int)queue, -1);
	atexit(vaccuming);
	
	//Wysyłam nazwe kolejki do servera
	CHECK(mq_send(server, name, 32, 1), -1);
	
	//Odbieram numer przydzielony mi przez serwer
	CHECK(mq_receive(queue, recv, 256, NULL), -1);
	id = recv[0];
	
	while(1) {
		//Wysylam informacje o gotowosci
		send[0] = id;
		CHECK(mq_send(server, send, 1, 2), -1);
		
		//Czekam na pracę (mtype = 2)
		CHECK(mq_receive(queue, recv, 256, NULL), -1);
		//Praca
		int pri = *(int*)recv;
		send[0] = id;
		*(int*)(send+1) = pri;
		send[sizeof(int)+1] = prime(pri);
		CHECK(mq_send(server, send, 2+sizeof(int), 3), -1);
	}
	return 0;
}
