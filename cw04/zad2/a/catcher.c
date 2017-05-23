#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int cntr = 0;

void sigusr1_hdl(int signum) {
	printf("CHLD: USR1\n");
	if(signum != SIGUSR1)
		return;
	cntr++;
	return;
}

void sigusr2_hdl(int signum) {
	printf("CHLD: USR2\n");
	if(signum != SIGUSR2)
		return;
	pid_t parent = getppid();
	for (int i=0; i<cntr; i++) {
		if(kill(parent, SIGUSR1) == -1) {
			perror("CHLD kill");
		}
		sleep(1);
	}
	if(kill(parent, SIGUSR2) == -1) {
		perror("CHLD kill");
	}
	exit(0);
	return;
}


int main()
{
	
	struct sigaction a;	
	a.sa_handler = sigusr1_hdl;
	a.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&a.sa_mask);
	sigaddset(&a.sa_mask, SIGUSR2);
	
	if(sigaction(SIGUSR1, &a, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	struct sigaction b;	
	b.sa_handler = sigusr2_hdl;
	b.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&b.sa_mask);
	sigaddset(&b.sa_mask, SIGUSR1);
	
	if(sigaction(SIGUSR2, &b, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	while(1)
		pause();
	return 0;
}

