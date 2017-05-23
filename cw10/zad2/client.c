#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //strcmp()
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>

#define UNIX_PATH_MAX		108
#define CLIENT_ID_LENGTH	16
#define MSG_LENGTH			256

#define CHECK(f, r)                       				         \
    do {                                                         \
        if (f == r)               				                 \
        {                                                        \
            perror(#f);                   		                 \
            exit(0);											 \
        }                                                        \
    } while (0)

struct msg_str {
	char id[CLIENT_ID_LENGTH];
	char msg[MSG_LENGTH];
	int msg_len;
};

int pipe1[2];//Pierwszy do odczytu(glowny), drugi do pisania (watek)
int pipe0[2];//Pierwszy do odczytu(watek), drugi do pisania (glowny)
int fd = -1;
pthread_t thr;

void sigint_hdl (int signum __attribute__((unused)) ) {
	pthread_cancel(thr);
	CHECK(close(pipe0[0]), -1);
	CHECK(close(pipe0[1]), -1);
	CHECK(close(pipe1[0]), -1);
	CHECK(close(pipe1[1]), -1);
	
	CHECK(shutdown(fd, SHUT_RDWR), -1);
	CHECK(close(fd), -1);
	printf("Koniec programu\n");
	exit(0);
}

void *thread(void *arg __attribute__((unused)) ) {

	char buf[CLIENT_ID_LENGTH+MSG_LENGTH+10];
	int cntr;
	struct pollfd fds[2] = {
							{pipe0[0], POLLIN, 0},
							{0, POLLIN, 0}
						};
						
	while (1) {
		CHECK(poll(fds, 2, -1), -1);
		
		if (fds[0].revents == POLLIN) {
			CHECK(read(pipe0[0], buf, CLIENT_ID_LENGTH+MSG_LENGTH+9), -1);
			printf("%s", buf);
		}
		
		if (fds[1].revents == POLLIN) {
			CHECK((cntr = read(0, buf, MSG_LENGTH-1)), -1);
			CHECK(write(pipe1[1], buf, cntr), -1);
		}
		
	}
	
	
}

int main(int argc, char **argv)
{

	//id, local, path
	//id, remote, ip, port
	char buf[CLIENT_ID_LENGTH+MSG_LENGTH+10];
	
	if (argc < 3) {
		printf("Zbyt malo argumentow\n");
		exit(1);
	}
	
	if (strcmp(argv[2], "local") == 0) {
		if (argc < 4) {
			printf("Zbyt malo argumentow\n");
			exit(1);
		}
		
		struct sockaddr_un unaddr;		
		unaddr.sun_family = AF_UNIX;
		strncpy(unaddr.sun_path, argv[3], UNIX_PATH_MAX);
		CHECK((fd = socket(AF_UNIX, SOCK_STREAM, 0)), -1);
		CHECK(connect(fd, (struct sockaddr*)&unaddr, sizeof(unaddr)), -1);
		printf("UNIX connect\n");

	}
	
	if (strcmp(argv[2], "remote") == 0) {
		if (argc < 5) {
			printf("Zbyt malo argumentow\n");
			exit(1);
		}
		
		char *p = argv[4];
		while (*p != 0) {
			if (isdigit((int)*p) == 0) {
				printf("Argument 4 powinien byc liczba\n");
				exit(1);
			}
			p++;
		}
		const uint16_t port = atoi(argv[4]);
		struct sockaddr_in netaddr;
		
		netaddr.sin_family = AF_INET;
		netaddr.sin_port = htons(port);
		CHECK(inet_aton(argv[3], &netaddr.sin_addr), 0);
		
		CHECK((fd = socket(AF_INET, SOCK_STREAM, 0)), -1);
		CHECK(connect(fd, (struct sockaddr*)&netaddr, sizeof(netaddr)), -1);
		printf("INET connect\n");
	}
	
	if (fd == -1) {
		printf("Something goes wrong\n");
	}

	signal(SIGINT, sigint_hdl);
	CHECK(pipe(pipe1), -1);
	CHECK(pipe(pipe0), -1);
	
	struct pollfd fds[2] = {//Pierwszy to socket, drugi to pipe wejście
							{fd, POLLIN | POLLRDHUP, 0},
							{pipe1[0], POLLIN, 0}
						}; 
	
	struct msg_str msg;
	int cntr;
	
	pthread_create(&thr, NULL, thread, NULL);
	//char buffer[32];
	
	while(1) {
		CHECK(poll(fds, 2, -1), -1); //To zwraca tez POLLIN gdy socket zamkniety ;(
									//By to naprawic mozemy napisac 2 linijki ponizej,
									//Albo uzyc POLLRDHUP z _GNU_SOURCE
		/*
		if (recv(fds[0].fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0)
			break;
		*/
		
		if(fds[0].revents & (POLLERR | POLLHUP | POLLRDHUP))
			break;
			
		if(fds[0].revents & POLLIN) {
			CHECK(recv(fd, (void*)&msg, sizeof(msg), 0), -1);
			cntr = sprintf(buf, "%s: %s", msg.id, msg.msg);
			cntr++;//NULL char
			CHECK(write(pipe0[1], buf, cntr), -1);
			fds[0].revents = 0;	
		}
		
		if (fds[1].revents & POLLIN) {
			CHECK((msg.msg_len = read(pipe1[0], msg.msg, MSG_LENGTH-1)), -1);
			msg.msg[msg.msg_len] = '\0';
			strcpy(msg.id, argv[1]);
			CHECK(send(fd, (void*)&msg, sizeof(msg), 0), -1);
			fds[1].revents = 0;
		}
	}
	
	kill(getpid(), SIGINT);
	return 0;
}

