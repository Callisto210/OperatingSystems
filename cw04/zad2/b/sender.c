#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


int recv = 0;
int signals = 0;
pid_t chld;

void sigusr1_hdl(int signum) {
	printf("PAR: USR1\n");
	if(signum != SIGUSR1)
		return;
	recv++;
	
	if(kill(chld, SIGUSR1) == -1) {
			perror("PAR kill");
	}
	
	return;
}

void sigusr1_hdl_alt(int signum) {
	if(signum != SIGUSR1)
		return;
	return;
}

void sigusr2_hdl(int signum) {
	printf("PAR: USR2\n");
	int ala;
	if(signum != SIGUSR2)
		return;
	printf("Wyslano: %u, Odebrano: %u", signals, recv);
	wait(&ala);
	exit(0);
	return;
}


int main(int argc, char **argv)
{
	struct sigaction b;	
	b.sa_handler = sigusr2_hdl;
	b.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&b.sa_mask);
	sigaddset(&b.sa_mask, SIGUSR1);
	
	if(sigaction(SIGUSR2, &b, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	if (argc != 2) {
		printf("Niewłaściwa ilość argumentów\n");
		return 0;
	}
	
	char *p = argv[1];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument powinien byc liczba\n");
			return 1;
		}
		p++;
	}
	
	signals = atoi(argv[1]);
	
	if ((chld = fork()) == 0)
		execlp("./catcher", "./catcher", NULL);
	
	sleep(1);
	
	sigset_t waitinfor;
	sigfillset(&waitinfor);
	sigdelset(&waitinfor, SIGUSR1);
	
	signal(SIGUSR1, sigusr1_hdl_alt);
	for (int i=0; i<signals; i++) {
		if(kill(chld, SIGUSR1) == -1) {
			perror("PAR kill");
		}
		sigsuspend(&waitinfor);
	}

	struct sigaction a;	
	a.sa_handler = sigusr1_hdl;
	a.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&a.sa_mask);
	sigaddset(&a.sa_mask, SIGUSR2);
	
	if(sigaction(SIGUSR1, &a, NULL) < 0) {
		perror("sigaction");
		return 1;
	}

	if(kill(chld, SIGUSR2) == -1) {
		perror("PAR kill");
	}
	
	while(1)
		pause();
	
	return 0;
}

