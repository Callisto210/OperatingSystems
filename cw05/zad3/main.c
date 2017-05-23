#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Zla ilosc argumentow\n");
		return 1;
	}
	
	char cmd[1024] = "ls -l | grep ^d > ";
	strncat(cmd, argv[1], 1024);
	
	FILE *p;
	p = popen(cmd, "r");
	if (p == NULL) {
		perror("popen");
		return 1;
	}
	
	if (pclose(p) == -1)
		perror("pclose");
	
	return 0;
}

