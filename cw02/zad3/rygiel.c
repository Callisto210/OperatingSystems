#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* options:
 * set read n
 * set write n
 * print
 * free n
 * read n
 * write n x
 * help
 * quit
 */
 
int is_num(char *p) {
	if (p == NULL) return 0;
	while (*p != 0) {
		if (isdigit((int)*p) == 0)
			return 0;
		p++;
	}
	return 1;
}

void wrong_cmd() {
	printf("Niewlasciwe polecenie. Sprobuj \"help\"\n");
	return;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Zbyt mala ilosc argumentów\n");
		return 1;
	}
		
	int plik; 
	if((plik = open(argv[1], O_RDWR)) == -1) {
		perror("open");
		return 1;
	}
	
	char buff[256];
	char *command[3];
	int n;
	char m;
	int r_length;
	struct flock l;
	
	if ((r_length = lseek(plik, 0l, SEEK_END)) == -1)
		perror("lseek");
	
	while(1) {
		
		printf("$ ");
		fgets(buff, 256, stdin);
		
		command[0] = strtok(buff, " \n");
		for (int i=1; i<3; i++)
			if ((command[i] = strtok(NULL, " \n")) == NULL)
				break;
				
		if (command[0] == NULL)
			continue;
		
		if(strcmp(command[0], "set") == 0) {
			if (command[1] == NULL || is_num(command[2]) == 0) {
				wrong_cmd();
				continue;
			}
			n = atoi(command[2]);
			
			l.l_whence = SEEK_SET;
			l.l_start = n;
			l.l_len = 1;
			l.l_type = F_UNLCK;
			
			if (strcmp(command[1], "read") == 0) l.l_type = F_RDLCK;
			if (strcmp(command[1], "write") == 0) l.l_type = F_WRLCK;
			
			if (l.l_type == F_UNLCK) {
				wrong_cmd();
				continue;
			}
			
			short backup = l.l_type;
			
			if(fcntl(plik, F_GETLK, &l) == -1) {
				perror("fcntl");
				continue;
			}
			
			if (l.l_type != F_UNLCK) {
				printf("Ten bajt jest juz zablokowany!\n");
				continue;
			}
			
			l.l_type = backup;
			
			if(fcntl(plik, F_SETLK, &l) == -1) {
				perror("fcntl");
				continue;
			}
			printf("Blokada ustawiona pomyslnie \n");
			continue;
		}

		if(strcmp(command[0], "print") == 0) {
			for (unsigned int i=0; i<r_length; i++) {
				l.l_whence = SEEK_SET;
				l.l_start = i;
				l.l_len = 1;
				l.l_type = F_WRLCK;
			
				if(fcntl(plik, F_GETLK, &l) == -1) {
					perror("fcntl");
					continue;
				}

				if(l.l_type != F_UNLCK)
					printf("%u	%u	%s\n", i, l.l_pid, (l.l_type == F_WRLCK) ? "F_WRLCK" : "F_RDLCK");
				
			}
			continue;
		}

		if(strcmp(command[0], "free") == 0) {
			if (is_num(command[1]) == 0) {
				wrong_cmd();
				continue;
			}
			
			n = atoi(command[1]);
			
			l.l_whence = SEEK_SET;
			l.l_start = n;
			l.l_len = 1;
			l.l_type = F_UNLCK;
			
			if(fcntl(plik, F_SETLK, &l) == -1) {
				perror("fcntl");
				continue;
			}
			
			continue;
		}

		if(strcmp(command[0], "read") == 0) {
			if (is_num(command[1]) == 0) {
				wrong_cmd();
				continue;
			}
			n = atoi(command[1]);
			
			l.l_whence = SEEK_SET;
			l.l_start = n;
			l.l_len = 1;
			l.l_type = F_RDLCK;
			
			if(fcntl(plik, F_GETLK, &l) == -1) {
				perror("fcntl");
				continue;
			}
			
			if(l.l_type == F_WRLCK) {
				printf("Nie moge odczytac bajtu - zablokowany\n");
				continue;
			}
			
			if (lseek(plik, n, SEEK_SET) == -1) {
				perror("lseek");
				continue;
			}
				
			if (read(plik, &m, 1) != 1) {
				perror("read");
				continue;
			}
				
			printf("%s@%u: %c\n", argv[1], n, m);
			continue;
		}

		if(strcmp(command[0], "write") == 0) {
			if (is_num(command[1]) == 0 || command[2] == NULL) {
				wrong_cmd();
				continue;
			}
			n = atoi(command[1]);
			
			l.l_whence = SEEK_SET;
			l.l_start = n;
			l.l_len = 1;
			l.l_type = F_WRLCK;
			
			if(fcntl(plik, F_GETLK, &l) == -1) {
				perror("fcntl");
				continue;
			}
			
			if(l.l_type != F_UNLCK) {
				printf("Nie moge pisac, bajt zablokowany\n");
				continue;
			}
			
			if (lseek(plik, n, SEEK_SET) == -1) {
				perror("lseek");
				continue;
			}
			
			if (write(plik, command[2], 1) != 1) {
				perror("write");
				continue;
			}
			
			continue;
		}

		if(strcmp(command[0], "help") == 0) {
			printf("Opcje: \nset read n \nset write n \nprint \nfree n \nread n \nwrite n x \nhelp \nquit \n");
			continue;
		}

		if(strcmp(command[0], "quit") == 0) {
			break;
		}
		wrong_cmd();
	}
	return 0;
}
