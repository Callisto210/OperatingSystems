#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define MSG_LENGTH 1024

int main(int argc, char **argv)
{
	int fifo;
	char msg[MSG_LENGTH];
	char str[200];
	size_t strcnt;
	char timestr[20];
	time_t timer;
	
	if(argc != 2) {
		printf("Zła ilośc argumentów\n");
		return 1;
	}
	
	if((fifo = open(argv[1], O_WRONLY)) == -1) {
		perror("open");
		return 1;
	}
	
	while (fgets(str, 200, stdin)) {
		timer = time(NULL);
		strftime(timestr, 20, "%T", localtime(&timer));
		strcnt = snprintf(msg, MSG_LENGTH, "%u-%s-%s", getpid(), timestr, str);
		if (write(fifo, msg, strcnt) == -1) {
			perror("write");
			return 1;
		}
	}
	
	return 0;
}



