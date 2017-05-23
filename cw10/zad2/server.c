#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h> //sockaddr_un
#include <netinet/in.h> //htons()
#include <string.h> //strncpy()
#include <ctype.h> //isdigit()
#include <poll.h> //poll()
#include <unistd.h> //close()
#include <time.h> //time()
#include <fcntl.h>
#include <signal.h>

#define UNIX_PATH_MAX		108
#define CLIENT_MAX			10
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

struct pollfd fd[CLIENT_MAX];
char *name;

void sigint_hdl (int signum __attribute__((unused)) ) {
	for (int i=0; i<CLIENT_MAX; i++) {
		if (fd[i].fd != -1) {
			CHECK(shutdown(fd[i].fd, SHUT_RDWR), -1);
			CHECK(close(fd[i].fd), -1);
		}
	}
	printf("Shutdown\n");
	CHECK(unlink(name), -1);
	exit(0);
}

struct client_str {
	char id[CLIENT_ID_LENGTH];
	time_t last_seen;
	int connected;
};

struct msg_str {
	char id[CLIENT_ID_LENGTH];
	char msg[MSG_LENGTH];
	int msg_len;
};

int main(int argc, char **argv)
{
	signal(SIGINT, sigint_hdl);
	//port, sciezka do socketu
	if (argc < 3) {
		printf("Zbyt malo argumentow\n");
		exit(1);
	}
	
	char *p = argv[1];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 1 powinien byc liczba\n");
			exit(1);
		}
		p++;
	}
	const uint16_t port = atoi(argv[1]);
	//Tworzymy gniazda
	for (int i=0; i<CLIENT_MAX; i++)
		fd[i].fd = -1;

	fd[0].fd = socket(AF_INET, SOCK_STREAM, 0);
	CHECK(fd[0].fd, -1);
	CHECK(setsockopt(fd[0].fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)), -1);
	
	fd[1].fd = socket(AF_UNIX, SOCK_STREAM, 0);
	CHECK(fd[1].fd, -1);
	
	//Przypisujemy gniazdom nzawy
	struct sockaddr_in netaddr;
	netaddr.sin_family = AF_INET;
	netaddr.sin_port = htons(port);
	netaddr.sin_addr.s_addr = INADDR_ANY;
	
	struct sockaddr_un unaddr;
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, argv[2], UNIX_PATH_MAX);
	name = argv[2];
	
	CHECK(bind(fd[0].fd, (struct sockaddr*)&netaddr, sizeof(netaddr)), -1);
	CHECK(bind(fd[1].fd, (struct sockaddr*)&unaddr, sizeof(unaddr)), -1);
	CHECK(listen(fd[0].fd, 100), -1);
	CHECK(listen(fd[1].fd, 100), -1);
	fd[0].events = POLLIN;
	fd[1].events = POLLIN;
	
	struct client_str *clients = calloc(CLIENT_MAX, sizeof(struct client_str));
	
	//struct client_str client_tmp;
	//socklen_t addr_len;
	
	
	//Gotowi do odbioru
	int poll_ret;
	ssize_t msg_size;
	struct msg_str msg;
	while(1) {
		CHECK((poll_ret = poll(fd, CLIENT_MAX, 10000)), -1);
		//Wywalamy przedawnionych
		time_t actual = time(NULL);
		for (int j=2; j<CLIENT_MAX; j++) 
			if((fd[j].fd != -1) && ((actual - clients[j].last_seen) >= 10) && (clients[j].connected == 0)) {
				CHECK(shutdown(fd[j].fd, SHUT_RDWR), -1);
				CHECK(close(fd[j].fd), -1);
				fd[j].fd = -1;
			}
		//Przechodzimy do odbioru
		if (poll_ret != 0) {
			
			if((fd[0].revents & (POLLERR | POLLHUP)) | (fd[1].revents & (POLLERR | POLLHUP)))
				break;
			
			if(fd[0].revents & POLLIN) {
				for (int j=2; j<CLIENT_MAX; j++) {
					if (fd[j].fd == -1) {
						printf("Akceptuje INET\n");
						fd[j].fd = accept(fd[0].fd, NULL, NULL);
						fd[j].events = POLLIN | POLLRDHUP;
						clients[j].connected = 0;
						clients[j].last_seen = time(NULL);
						break;
					}
				}
			}
			
			if(fd[1].revents & POLLIN) {
				for (int j=2; j<CLIENT_MAX; j++) {
					if (fd[j].fd == -1) {
						printf("Akceptuje UNIX\n");
						fd[j].fd = accept(fd[1].fd, NULL, NULL);
						fd[j].events = POLLIN | POLLRDHUP;
						clients[j].connected = 0;
						clients[j].last_seen = time(NULL);
						break;
					}
				}
			}
			
			for (int i = 2; i<CLIENT_MAX; i++) {

				if(fd[i].revents & POLLRDHUP) {
					CHECK(shutdown(fd[i].fd, SHUT_RDWR), -1);
					CHECK(close(fd[i].fd), -1);
					fd[i].fd = -1;
					printf("POLLRDHUP\n");

				}

				if(fd[i].revents & POLLIN) {
					if(fd[i].fd == -1)
						break;
					clients[i].connected = 1;
					msg_size = recv(fd[i].fd, (void*)&msg, sizeof(msg), 0);
					CHECK(msg_size, -1);
					
					printf("%s: %s", msg.id, msg.msg);
					strncpy(clients[i].id, msg.id, CLIENT_ID_LENGTH);
					
					//wysylamy
					for(int j = 2; j<CLIENT_MAX; j++) {
						if (fd[j].fd != -1) {
							CHECK(send(fd[j].fd, (void*)&msg, sizeof(msg), 0), -1);
						}
							
					}
				}	
				fd[i].revents = 0;
			}
		}
	}
	
	kill(getpid(), SIGINT);
	return 0;
}

