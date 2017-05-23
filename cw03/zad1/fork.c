#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>

unsigned int *counter;

int main(void)
{
	struct tms czas;
	long ticksperse = sysconf(_SC_CLK_TCK);
	clock_t real = times(NULL);
	clock_t *realchldsum;
	
	if ((counter = mmap(NULL, 
						sizeof(unsigned int), 
						PROT_READ | PROT_WRITE, 
						MAP_ANONYMOUS | MAP_SHARED, 
						-1, 0)) == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	
	if ((realchldsum = mmap(NULL, 
						sizeof(clock_t), 
						PROT_READ | PROT_WRITE, 
						MAP_ANONYMOUS | MAP_SHARED, 
						-1, 0)) == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	
	*counter = 0;
	*realchldsum = 0;
	
	pid_t chld;
	int status;
		for(int i=0; i<1000000; i++) {
			chld = fork();
			if(chld == 0) {
				clock_t realchld = times(NULL);
				(*counter)++;
				(*realchldsum) += times(NULL) - realchld;
				_exit(0);
			}
			else
			if (chld == -1)
				perror("fork");
			wait(&status);		
		}
	
	times(&czas);
	printf("child, fork, %u, %f, %f, %f\n", *counter, (double)(*realchldsum)/(double)ticksperse,(double)(czas.tms_cstime)/(double)ticksperse, (double)(czas.tms_cutime)/(double)ticksperse);
	printf("mother, fork, %u, %f, %f, %f\n", *counter, (double)(times(NULL) - real)/(double)ticksperse, (double)(czas.tms_stime)/(double)ticksperse, (double)(czas.tms_utime)/(double)ticksperse);
	munmap(counter, sizeof(unsigned int));
	munmap(realchldsum, sizeof(clock_t));
	return 0;
}

