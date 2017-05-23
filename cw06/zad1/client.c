#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h>

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

int prime(int p) {
	if(p == 0 || p == 1)
		return 0;
	for (int i=2; i<=sqrt(p); i++)
		if(p%i == 0)
			return 0;
	return 1;
}

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Zla ilosc argumentow");
		exit(0);
	}
	
	key_t key = ftok(argv[1], argv[2][0]);
	CHECK(key, -1);
	
	int server = msgget(key, IPC_CREAT | 0600);
	CHECK(server, -1);
	
	queue = msgget(IPC_PRIVATE, 0600);
	CHECK(queue, -1);
	atexit(vaccuming);
	
	//Wysyłam numer mojej prywatnej kolejki (mtype = 1)
	struct intmsg imsg = {1, queue};
	CHECK(msgsnd(server, &imsg, sizeof(int), 0), -1);
	
	//Odbieram numer przydzielony mi przez serwer (mtype = 1)
	int id;
	CHECK(msgrcv(queue, &imsg, sizeof(int), 1, 0), -1);
	id = imsg.val;
	
	while(1) {
		//Wysylam informacje o gotowosci (mtype = 2)
		imsg.mtype = 2;
		imsg.val = id;
		CHECK(msgsnd(server, &imsg, sizeof(int), 0), -1);
		
		//Czekam na pracę (mtype = 2)
		CHECK(msgrcv(queue, &imsg, sizeof(int), 2, 0), -1);
		//Praca
		struct primemsg pmsg;
		pmsg.mtype = 3;
		pmsg.id = id;
		pmsg.num = imsg.val;
		pmsg.prime = prime(imsg.val);
		CHECK(msgsnd(server, &pmsg, 3*sizeof(int), 0), -1);
	}
	return 0;
}
