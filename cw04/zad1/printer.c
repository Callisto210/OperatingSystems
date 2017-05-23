#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

int max;
int now;
int rev;
int dir;

void sigtstp_hdl(int signum) {
	static int cntr = 1;
	
	if((cntr%3) == 1)
		rev = 1;
	else
		rev = 0;
		
	if(now == 1)
		dir = 1;
	else if(now == max)
		dir = 0;
		
	if (cntr%3 == 0) {	
		if (dir)
			now++;
		else
			now--;
	}
		
	cntr++;	
	return;
}

void sigint_hdl(int signum) {
	printf("Odebrano sygnal SIGINT\n");
	exit(signum);
	return;
}


int main (int argc, char **argv) {
	signal(SIGINT, sigint_hdl);
	
	if (argc != 3) {
		printf("Niewłaściwa ilość argumentów\n");
		return 0;
	}
	
	now = 1;
	rev = 0;
	dir = 1;
	
	char *p = argv[2];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 2 powinien byc liczba\n");
			return 1;
		}
		p++;
	}
	
	max = atoi(argv[2]);
	
	int l = strlen(argv[1]);
	
	struct sigaction a;	
	a.sa_handler = sigtstp_hdl;
	a.sa_flags = 0; //flagi modyfikujące
	sigemptyset(&a.sa_mask);
	
	if(sigaction(SIGTSTP, &a, NULL) < 0) {
		perror("sigaction");
		return 1;
	}
	
	while(1) {
		for (int j=0; j<now; j++) {
			if (rev)
				for (int i=l-1; i>=0; i--) printf("%c", argv[1][i]);
			else
				printf("%s", argv[1]);
		}
			
		printf("\n");
	}
	return 0;
	
}
