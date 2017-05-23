#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

unsigned int len = 0;

void list (const char *root, unsigned short pattern) {
	DIR *f;
	if ((f = opendir(root)) == NULL) {
		/* perror("opendir"); By uniknąc niepotrzebnych komunikatów */
		return;
	}
	struct dirent *d;
	struct stat s;
	
	char *next_dir = calloc(strlen(root)+257, sizeof(char));
	strcpy(next_dir, root);
	strcat(next_dir, "/");
	
	int l = strlen(next_dir);
	
	while ((d = readdir(f)) != NULL) {
		next_dir[l] = '\0';
		strcat(next_dir, d->d_name);
		
		switch (d->d_type) {
			case DT_DIR:
				if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
					continue;
				list(next_dir, pattern);
				break;
				
			case DT_REG:
				stat(next_dir, &s);
				if((s.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) == pattern)
					printf("%s %li %s", next_dir+len, s.st_size, ctime(&s.st_atime));
				break;
		}
	}
	free(next_dir);
	closedir(f);
	return;
}

int main(int argc, char **argv)
{	
	if (argc < 3) {
		printf("Zbyt mala ilość ilość argumentow\n");
		return 1;
	}
	
	unsigned short pattern = 0;
	
	if (argv[2][0] == 'r')
		pattern |= S_IRUSR;
	if (argv[2][1] == 'w')
		pattern |= S_IWUSR;
	if (argv[2][2] == 'x')
		pattern |= S_IXUSR;
	if (argv[2][2] == 's')
		pattern |= S_ISUID;	
		
	if (argv[2][3] == 'r')
		pattern |= S_IRGRP;
	if (argv[2][4] == 'w')
		pattern |= S_IWGRP;
	if (argv[2][5] == 'x')
		pattern |= S_IXGRP;
	if (argv[2][2] == 's')
		pattern |= S_ISGID;
		
	if (argv[2][6] == 'r')
		pattern |= S_IROTH;
	if (argv[2][7] == 'w')
		pattern |= S_IWOTH;
	if (argv[2][8] == 'x')
		pattern |= S_IXOTH;
		
	char *p = argv[1];
	while(*p != 0) {
		len++;
		p++;
	}
	len++;
	
	list(argv[1], pattern);
	return 0;
}

