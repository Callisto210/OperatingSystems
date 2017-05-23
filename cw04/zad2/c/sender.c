#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


int recv = 0;
int signals = 0;

void SIGRTMIN_hdl(int signum) {
	printf("PAR: USR1\n");
	if(signum != SIGRTMIN)
		return;
	recv++;
	return;
}

void SIGRTMAX_hdl(int signum) {
	printf("PAR: USR2\n");
	if(signum != SIGRTMAX)
		return;
	printf("Wyslano: %u, Odebrano: %u\n", signals, recv);
	sleep(1);
	exit(0);
	return;
}

void sigchld_hdl(int signum) {
	if(signum != SIGCHLD)
		return;
	//printf("End of Child\n");
	int ala;
	wait(&ala);
	return;
}

int main(int argc, char **argv)
{
	struct sigaction a;	
	a.sa_handler = SIGRTMIN_hdl;
	a.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&a.sa_mask);
	sigaddset(&a.sa_mask, SIGRTMAX);
	
	if(sigaction(SIGRTMIN, &a, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	struct sigaction b;	
	b.sa_handler = SIGRTMAX_hdl;
	b.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&b.sa_mask);
	
	if(sigaction(SIGRTMAX, &b, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	signal(SIGCHLD, sigchld_hdl);
	
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
	
	pid_t chld;
	if ((chld = fork()) == 0)
		execlp("./catcher", "./catcher", NULL);
	
	sleep(1);
	for (int i=0; i<signals; i++) {
		if(kill(chld, SIGRTMIN) == -1) {
			perror("PAR kill");
		}
	}

	if(kill(chld, SIGRTMAX) == -1) {
		perror("PAR kill");
	}
	
	while(1)
		pause();
	
	return 0;
}

