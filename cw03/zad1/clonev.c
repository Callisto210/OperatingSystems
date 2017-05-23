#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>

#define STACK_SIZE 4096

clock_t realchldsum = 0;

int inc (void *counter) {
	clock_t realchld = times(NULL);
	(*(unsigned int *)counter)++;
	realchldsum += times(NULL) - realchld;
	return 0;
}

int main(void)
{
	struct tms czas;
	long ticksperse = sysconf(_SC_CLK_TCK);
	clock_t real = times(NULL);
	
	unsigned char *stack;
	
	if ((stack = mmap(	NULL, 
						STACK_SIZE, 
						PROT_READ | PROT_WRITE, 
						MAP_ANONYMOUS | MAP_SHARED, 
						-1, 0)) == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	
	unsigned int counter = 0;
	int status;
		for(int i=0; i<1000000; i++) {
			if (clone(inc, stack+STACK_SIZE-1, CLONE_VM | CLONE_VFORK | SIGCHLD, &counter) == -1)
				perror("clone");
			wait(&status);
		}
	times(&czas);
	printf("child, clonev, %u, %f, %f, %f\n", counter, (double)(realchldsum)/(double)ticksperse,(double)(czas.tms_cstime)/(double)ticksperse, (double)(czas.tms_cutime)/(double)ticksperse);
	printf("mother, clonev, %u, %f, %f, %f\n", counter, (double)(times(NULL) - real)/(double)ticksperse, (double)(czas.tms_stime)/(double)ticksperse, (double)(czas.tms_utime)/(double)ticksperse);

	munmap(stack, STACK_SIZE);
	return 0;
}
