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
#define CLIENT_MAX			256
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

int running = 1;

void sigint_hdl (int signum) {
	running = 0;
	return;
}

struct client_str {
	int used;
	char id[CLIENT_ID_LENGTH];
	sa_family_t family;
	union {
		struct sockaddr_in netaddr;
		struct sockaddr_un unixaddr;
	} addr;
	time_t last_seen;
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
	struct pollfd fd[2];
	fd[0].fd = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK(fd[0].fd, -1);
	
	fd[1].fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	CHECK(fd[1].fd, -1);
	
	//Przypisujemy gniazdom nzawy
	struct sockaddr_in netaddr  = {AF_INET, htons(port), INADDR_ANY};
	struct sockaddr_un unaddr;
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, argv[2], UNIX_PATH_MAX);
	
	CHECK(bind(fd[0].fd, (struct sockaddr*)&netaddr, sizeof(netaddr)), -1);
	CHECK(bind(fd[1].fd, (struct sockaddr*)&unaddr, sizeof(unaddr)), -1);
	
	struct client_str *clients = calloc(CLIENT_MAX, sizeof(struct client_str));
	
	struct client_str client_tmp;
	socklen_t addr_len;
	
	
	//Gotowi do odbioru
	int poll_ret;
	ssize_t msg_size;
	struct msg_str msg;
	while(running) {
		fd[0].events = POLLIN;
		fd[1].events = POLLIN;
		CHECK((poll_ret = poll(fd, 2, 30000)), -1);
		//printf("Minelo 30 sekund\n");
		//Wywalamy przedawnionych
		time_t actual = time(NULL);
		for (int j=0; j<CLIENT_MAX; j++) 
			if((actual - clients[j].last_seen) >= 30) 
				clients[j].used = 0;
		//Przechodzimy do odbioru
		if (poll_ret != 0) {
			for (int i = 0; i<2; i++) {
				switch(fd[i].revents) {
					case POLLIN:
					if(i == 0) {
						addr_len = sizeof(struct sockaddr_in);
						msg_size = recvfrom(fd[i].fd, (void*)&msg, sizeof(msg), 0, (struct sockaddr*)&(client_tmp.addr.netaddr), &addr_len);
						CHECK(msg_size, -1);
						client_tmp.family = AF_INET;
					}
					else 
					if(i == 1) {
						addr_len = sizeof(struct sockaddr_un);
						msg_size = recvfrom(fd[i].fd, (void*)&msg, sizeof(msg), 0, (struct sockaddr*)&(client_tmp.addr.unixaddr), &addr_len);
						CHECK(msg_size, -1);
						client_tmp.family = AF_UNIX;
						client_tmp.addr.unixaddr.sun_family = AF_UNIX;
					}
					else
						break;
					printf("%s: %s", msg.id, msg.msg);
					strncpy(client_tmp.id, msg.id, CLIENT_ID_LENGTH);
					client_tmp.used = 1;
					client_tmp.last_seen = time(NULL);
					
					int inserted = 0;
					for (int j=0; j<CLIENT_MAX; j++) { //Sprawdzamy czy istnieje już taki
						if(strcmp(clients[j].id, client_tmp.id) == 0 && clients[j].used == 1) {
							memcpy((void*)&clients[j], (void*)&client_tmp, sizeof(struct client_str));
							inserted = 1;
							break;
						}
					}
					
					if (inserted == 0) {
						for (int j=0; j<CLIENT_MAX; j++) {
							if (clients[j].used == 0) {
								memcpy((void*)&clients[j], (void*)&client_tmp, sizeof(struct client_str));
								inserted = 1;
								break;
							}
						}
					}
					
					if (inserted == 0)
						printf("Something goes wrong :(\n");
					
					//wysylamy
					for(int j = 0; j<CLIENT_MAX; j++) {
						if (clients[j].used != 0) {
							switch (clients[j].family) {
								case AF_INET:
								CHECK(sendto(fd[0].fd, (void*)&msg, sizeof(msg), 0, (struct sockaddr*)&clients[j].addr.netaddr, sizeof(clients[j].addr.netaddr)), -1);
								break;
								
								case AF_UNIX:
								printf("%s\n", clients[j].addr.unixaddr.sun_path);
								CHECK(sendto(fd[1].fd, (void*)&msg, sizeof(msg), 0, (struct sockaddr*)&clients[j].addr.unixaddr, sizeof(clients[j].addr.unixaddr)), -1);
								break;
							}
						}
					}
					break;//POLLIN
				}
				fd[i].revents = 0;
			}
		}
	}
	
	CHECK(shutdown(fd[0].fd, SHUT_RDWR), -1);
	CHECK(shutdown(fd[1].fd, SHUT_RDWR), -1);
	
	CHECK(close(fd[0].fd), -1);
	CHECK(close(fd[1].fd), -1);
	
	CHECK(unlink(argv[2]), -1);
	
	return 0;
}

