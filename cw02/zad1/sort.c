#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int compare (const char *a, const char* b) {
	if((unsigned int)*a < (unsigned int)*b)
		return 1;
	return 0;
}

void lib (const char *filename, int r_length) {
	
	FILE *plik;
	if ((plik = fopen(filename, "r+b")) == NULL) {
		perror("fopen");
		return;
	}
	
	char *buffer_one = malloc(r_length);
	char *buffer_two = malloc(r_length);
	
	if (buffer_one == NULL || buffer_two == NULL)
		return;
	
	fseek(plik, 0L, SEEK_END);
	if (ferror(plik)) {
		printf ("fseek: failed");
	}
	int r_num = ftell(plik)/r_length;
	
	for (int i=1; i<r_num; i++ ) {
		
		fseek(plik, i*r_length, SEEK_SET);
		if (ferror(plik)) {
			printf ("fseek: failed");
		}
		if (fread(buffer_one, 1, r_length, plik) != r_length)
			perror("fread");
		
		for (int j=i-1; j>=0; j--) {
			
			fseek(plik, j*r_length, SEEK_SET);
			if (ferror(plik)) {
				printf ("fseek: failed");
			}
			if (fread(buffer_two, 1, r_length, plik) != r_length)
				perror("fread");
				
			if (compare(buffer_one, buffer_two)) { //jesli prawda, to buffer_one > buffer_two
				if (fwrite(buffer_two, 1, r_length, plik) != r_length)
					perror("fwrite");
			}
			else {
				if (fwrite(buffer_one, 1, r_length, plik) != r_length)
					perror("fwrite");
				break;
			}
			if (j == 0) {
				fseek(plik, 0L, SEEK_SET);
				if (fwrite(buffer_one, 1, r_length, plik) != r_length)
					perror("fwrite");
			}	
		}
	}
	
	free(buffer_one);
	free(buffer_two);
	return;
}

void sys (const char *filename, int r_length) {
	
	int plik;
	if ((plik = open(filename, O_RDWR | O_EXCL)) == -1) {
		perror("open");
		return;
	}
	
	char *buffer_one = malloc(r_length);
	char *buffer_two = malloc(r_length);
	
	if (buffer_one == NULL || buffer_two == NULL)
		return;
		
	int r_num;
	if ((r_num = lseek(plik, 0L, SEEK_END)) == -1)
		perror("lseek");
		
	r_num /= r_length;
	
	for (int i=1; i<r_num; i++ ) {
		
		if (lseek(plik, i*r_length, SEEK_SET) == -1)
			perror("lseek");
			
		if (read(plik, buffer_one, r_length) != r_length)
			perror("read");
		
		for (int j=i-1; j>=0; j--) {
			
			if (lseek(plik, j*r_length, SEEK_SET) == -1)
				perror("lseek");
				
			if (read(plik, buffer_two, r_length) != r_length)
				perror("read");
				
			if (compare(buffer_one, buffer_two)) { //jesli prawda, to buffer_one > buffer_two
				if (write(plik, buffer_two, r_length) != r_length)
					perror("write");
			}
			else {
				if (write(plik, buffer_one, r_length) != r_length)
					perror("write");
				break;
			}
			if (j == 0) {
				if (lseek(plik, 0L, SEEK_SET) == -1)
					perror("lseek");
				if (write(plik, buffer_one, r_length) != r_length)
					perror("write");
			}	
		}
	}
	
	free(buffer_one);
	free(buffer_two);
	return;
	
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		printf("Niewystarczająca ilośc argumentów\n");
		return 1;
	}
	
	char *p = argv[2];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 2 powinien byc liczba\n");
			return 1;
		}
		p++;
	}
		
	int r_length = atoi(argv[2]);
	r_length++; //Bo wiersz ma znak końca lini
	
	if (strcmp(argv[3], "lib") == 0) {
		lib(argv[1], r_length);
		return 0;
	}
	else
	if (strcmp(argv[3], "sys") == 0) {
		sys(argv[1], r_length);
		return 0;
	}
	else {
		printf("Zly wybor zestawu funkcji\n");
		return 1;
	}
	
	return 0;
}

