#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int cntr = 0;

void SIGRTMIN_hdl(int signum) {
	printf("CHLD: USR1\n");
	if(signum != SIGRTMIN)
		return;
	cntr++;
	return;
}

void SIGRTMAX_hdl(int signum) {
	printf("CHLD: USR2\n");
	if(signum != SIGRTMAX)
		return;
	pid_t parent = getppid();
	for (int i=0; i<cntr; i++) {
		if(kill(parent, SIGRTMIN) == -1) {
			perror("CHLD kill");
		}
	}
	if(kill(parent, SIGRTMAX) == -1) {
		perror("CHLD kill");
	}
	exit(0);
	return;
}


int main()
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
	sigaddset(&b.sa_mask, SIGRTMIN);
	
	if(sigaction(SIGRTMAX, &b, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	while(1)
		pause();
	return 0;
}

