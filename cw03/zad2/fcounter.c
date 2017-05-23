#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	char path[PATH_MAX] = ".";
	int verbose = 0;
	if (argc > 3) {
		printf("Zbyt wiele argumentow\n");
		_exit(0);
	}
	
	for (int i=1; i<argc; i++) {
		if (strcmp("-v", argv[i]) == 0)
			verbose = 1;
		else
		if (strcmp("-s", argv[i]) == 0)
			verbose = -1;
		else
			strncpy(path, argv[i], PATH_MAX);
	}
	
	DIR *f;
	if ((f = opendir(path)) == NULL) {
		/* perror("opendir"); By uniknąc niepotrzebnych komunikatów */
		exit(0);
	}
	
	int status;
	unsigned int counter = 0;
	unsigned int myfiles = 0;
	struct dirent *d;
	pid_t chld;
	int childrens = 0;
	
	char *next_dir = calloc(PATH_MAX, sizeof(char));
	strncpy(next_dir, path, PATH_MAX);
	strcat(next_dir, "/");
	
	int l = strlen(next_dir);
	
	while ((d = readdir(f)) != NULL) {
		next_dir[l] = '\0';
		strcat(next_dir, d->d_name);
		
		switch (d->d_type) {
			case DT_DIR:
				if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
					continue;
				childrens++;
				chld = fork();
				if(chld == 0) {
					if (verbose == 1) {
						if (execl(argv[0], argv[0], "-v", next_dir, NULL) == -1)
							perror("execlp");
					}
					else {
						if (execl(argv[0], argv[0], "-s", next_dir, NULL) == -1)
							perror("execlp");
					}
				}
				else
				if (chld == -1)
					perror("fork");
				break;
				
			case DT_REG:
				counter++;
				break;
		}
	}
	myfiles = counter;
	for (int i=0; i<childrens; i++) {
		wait(&status);
		status >>=8; //Osiem najmłodszych bitów zawiera informacje o tym jak proces skończył
		counter+=status;
	}
				
	free(next_dir);
	closedir(f);

	if(verbose == 1)
		printf("%s %u, %u\n", path, myfiles, counter);
	else
	if (verbose == 0)
		printf("%u plikow\n", counter);
	exit(counter);
}
