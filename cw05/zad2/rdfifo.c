#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>

#define MSG_LENGTH 1024

int main(int argc, char **argv)
{
	int fifo;
	char msg[MSG_LENGTH];
	char timestr[20];
	time_t timer;
	int msglen;
	
	if(argc != 2) {
		printf("Zła ilośc argumentów\n");
		return 1;
	}
	
	if(mkfifo(argv[1],  0600) == -1) {
		perror("mkfifo");
		return 1;
	}
	
	if((fifo = open(argv[1], O_RDONLY)) == -1) {
		perror("open");
		return 1;
	}
	
	struct pollfd fifo_poll = {
								.fd = fifo,
								.events = POLLIN
								};
	
	
	while (poll(&fifo_poll, 1, -1) != -1) {
		switch(fifo_poll.revents) {
			case POLLIN:
				msglen = read(fifo, msg, MSG_LENGTH-1);
				if (msglen == -1) {
					perror("read");
					return 1;
				}
				msg[msglen] = '\0';
				timer = time(NULL);
				strftime(timestr, 20, "%T", localtime(&timer));
				printf("%s-%s", timestr, msg);
				break;
			case POLLHUP:
				if(close(fifo) == -1) {
					perror("close");
					return 1;
				}
				if((fifo = open(argv[1], O_RDONLY)) == -1) {
					perror("open");
					return 1;
				}
				fifo_poll.fd = fifo;
				break;
		}
	}
	
	
	
	return 0;
}

