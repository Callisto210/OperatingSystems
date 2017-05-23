#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <errno.h>


#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

struct intmsg 
{ 
	long mtype;
	int val;		
};

struct primemsg 
{ 
	long mtype;
	int id;
	int num;
	int prime;		
};

int queue;

void vaccuming(void) {
	CHECK(msgctl(queue, IPC_RMID, NULL), -1);
	return;
}

int main (int argc, char **argv) {
	srand(time(NULL));
	if (argc != 3) {
		printf("Zła ilośc argumentów");
		exit(0);
	}
	
	int client_cntr = 0;
	int client[128];
	
	struct intmsg imsg;
	struct primemsg pmsg;
	
	key_t key = ftok(argv[1], argv[2][0]);
	CHECK(key, -1);
	
	queue = msgget(key, IPC_CREAT | 0600);
	CHECK(queue, -1);
	atexit(vaccuming);	
	
	while(1) {
		CHECK(msgrcv(queue, &imsg, sizeof(int), 3, MSG_EXCEPT), -1);
		switch (imsg.mtype) {
			case 1: //Klient sie zglosil (mtype = 1)
			client[client_cntr] = imsg.val;
			
			//Odbijamy klientowi jego numer (mtype = 1)
			imsg.mtype = 1;
			imsg.val = client_cntr;
			CHECK(msgsnd(client[client_cntr], &imsg, sizeof(int), 0), -1);
			client_cntr++;
			break;
			
			case 2: //Klient zglasza gotowosc
			//Sprawdzmy czy nie ma wyniku
			if(msgrcv(queue, &pmsg, 3*sizeof(int), 3, IPC_NOWAIT) == -1) {
				if(errno != ENOMSG)
					CHECK(1, 1);
			} else {
				if(pmsg.prime)
					printf("Liczba pierwsza: %u (klient: %u)\n", pmsg.num, pmsg.id);
			}
			
			int id = imsg.val;
			imsg.mtype = 2;
			imsg.val = rand();
			CHECK(msgsnd(client[id], &imsg, sizeof(int), 0), -1);
			
			break;
		}
	}
	return 0;
}
