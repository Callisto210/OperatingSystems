#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

int main(int argc, char **argv)
{	
	
	if (argc != 2) {
		printf("Zbyt mała ilość argumentów\n");
		return 1;
	}
	
	char *z = argv[1];
	while (*z != 0) {
		if (isdigit((int)*z) == 0) {
			printf("Argument 2 powinien byc liczba\n");
			return 1;
		}
		z++;
	}
	
	int p[2];

	if(pipe(p) == -1) {
		perror("pipe");
		return 0;
	}	

	if(fork() == 0) {
		close(p[0]);
		dup2(p[1], STDOUT_FILENO);
		if(execlp("tr", "tr", "'[:lower:]'", "'[:upper:]'",NULL) == -1)
			perror("execlp");
		exit(0);
	}

	pid_t pid = fork();
	if(pid == 0) {
		close(p[1]);
		dup2(p[0], STDIN_FILENO);
		if(execlp("fold", "fold", "-w", argv[1], NULL) == -1)
			perror("execlp");
		exit(0);
	}

	int ala;
	wait(&ala);
	close(p[1]);
	wait(&ala);
	
	return 0;
}

