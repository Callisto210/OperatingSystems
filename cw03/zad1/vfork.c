#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>

int main(void)
{
	struct tms czas;
	long ticksperse = sysconf(_SC_CLK_TCK);
	clock_t real = times(NULL);
	clock_t realchldsum = 0;
	
	int status;
	unsigned int counter = 0;
	pid_t chld;
		for(int i=0; i<1000000; i++) {
			chld = vfork();
			if(chld == 0) {
				clock_t realchld = times(NULL);
				counter++;
				realchldsum += times(NULL) - realchld;
				_exit(0);
			}
			else
			if (chld == -1)
				perror("vfork");
			wait(&status);
			
		}
	times(&czas);
	printf("child, vfork, %u, %f, %f, %f\n", counter, (double)(realchldsum)/(double)ticksperse,(double)(czas.tms_cstime)/(double)ticksperse, (double)(czas.tms_cutime)/(double)ticksperse);
	printf("mother, vfork, %u, %f, %f, %f\n", counter, (double)(times(NULL) - real)/(double)ticksperse, (double)(czas.tms_stime)/(double)ticksperse, (double)(czas.tms_utime)/(double)ticksperse);

	return 0;
}

