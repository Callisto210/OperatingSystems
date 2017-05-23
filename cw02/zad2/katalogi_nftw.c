#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>
#include <time.h>
#include <string.h>

unsigned short pattern = 0;

int display_info(const char *fpath, const struct stat *sb,
					int tflag, struct FTW *ftwbuf) {
	if(tflag != FTW_F)
		return 0;
						
	if((sb->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != pattern)
		return 0;
		
	char *rpath = realpath(fpath, NULL);
	printf("%s %li %s", rpath, sb->st_size, ctime(&sb->st_atime));
	free(rpath);

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Zbyt mala ilość ilość argumentow\n");
		return 1;
	}
	
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

	nftw(argv[1], display_info, 100, FTW_PHYS | FTW_DEPTH);
	
	return 0;
}

