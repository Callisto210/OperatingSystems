#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

int main(int argc, char **argv)
{
	srand(time(NULL));
	
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
	
	p = argv[3];
	while (*p != 0) {
		if (isdigit((int)*p) == 0) {
			printf("Argument 3 powinien byc liczba\n");
			return 1;
		}
		p++;
	}
	
	int r_length = atoi(argv[2]);
	int r_num = atoi(argv[3]);
	
	char *buffer = malloc(r_length+1); //Dodatkowo rezerwujemy miejsce na znak końca lini
	buffer[r_length] = '\n';
	
	FILE *plik;
	if ((plik = fopen(argv[1], "wb")) == NULL) {
		printf("Jakis problem z plikiem\n");
		return 1;
	}
	
	for(int i=0; i<r_num; i++) {
		for (int i=0; i<r_length; i++)
			buffer[i] = rand()%42+48;
		fwrite(buffer, 1, r_length+1, plik);
	}
	
	fclose(plik);
	return 0;
}

