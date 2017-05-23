#define DLL_BUILD
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <list.h>
#include <string.h>
#include <dlfcn.h>

void * lib;
list *(*create_list)();
void (*add_node)(list *, char *, char *, int, int, int, char *, char *, char *);
void (*sort_list)(list *, int (*)(data_t *, data_t *));
void (*delete_list)(list *);
char *error;

void load() {
	lib = dlopen("./../zad1/lib/liblist.so", RTLD_LAZY);
	error = dlerror();
	if (error) {
		printf("%s'\n", error);
		exit(1);
	}
	
	create_list = dlsym(lib, "create_list");
	error = dlerror();
	if (error) {
		printf("%s'\n", error);
		exit(1);
	}
	
	add_node = dlsym(lib, "add_node");
	error = dlerror();
	if (error) {
		printf("%s'\n", error);
		exit(1);
	}
	
	sort_list = dlsym(lib, "sort_list");
	error = dlerror();
	if (error) {
		printf("%s'\n", error);
		exit(1);
	}
	
	delete_list = dlsym(lib, "delete_list");
	error = dlerror();
	if (error) {
		printf("%s'\n", error);
		exit(1);
	}
	
	return;
}

int compare (data_t *a, data_t *b) {
	int cmp;
	if (a->lastname != NULL && b->lastname != NULL) {
		cmp = strcmp(a->lastname, b->lastname);
	
		if (cmp < 0)
			return 1;
		else
		if (cmp > 0)
			return 0;
	}
	
	if (a->name != NULL && b->name != NULL) {
		cmp = strcmp(a->name, b->name);
	
		if (cmp < 0)
			return 1;
	}
	return 0;
}

void print_list (list *l) {
	node *n = l->first;
	while (n != NULL) {
		printf("%s %s\n", n->data.name, n->data.lastname);
		n = n->next;
	}
	return;
}

int main (int argc, char **argv) {
	
	int time = 1;
	if (argc >= 2 && strcmp("--test", argv[1]) == 0)
		time = 0;
		
	struct tms start;
	times(&start);
	clock_t real_start = clock();
	
	load();
	
	struct tms last;
	clock_t real_last;
	
	struct tms t;
	clock_t c;
	
	if (time) {
		times(&t);
		c = clock();
		printf("START: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_start)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - start.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - start.tms_stime)/(double)CLOCKS_PER_SEC );
		last = t;
		real_last = c;
	}
	
	list *l = create_list();
	
	add_node(l, "AB", "AA", 29, 02, 1996, "ul. 3-go Maja", "123456789", "sgsdf");
	add_node(l, "AC", "AB", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AD", "ABC", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AE", "ABC", 432, 324, 3, NULL, "53346435", "sfgsdg");
	add_node(l, "AF", "AC", 432, 324, 3, "dfgsdg", NULL, "sfgsdg");
	add_node(l, "AG", "E", 432, 324, 3, "dfgsdg", "53346435", NULL);
	add_node(l, "AGG", "EC", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AGT", "EA", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AGT", "EG", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AGT", "EZ", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AGD", "EZ", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	add_node(l, "AGD", "EZ", 432, 324, 3, "dfgsdg", "53346435", "sfgsdg");
	
	//print_list(l);
	
	if (time) {
		times(&t);
		c = clock();
		printf("START: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_start)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - start.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - start.tms_stime)/(double)CLOCKS_PER_SEC );
		printf("LAST: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_last)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - last.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - last.tms_stime)/(double)CLOCKS_PER_SEC );
		last = t;
		real_last = c;
	}
	
	sort_list(l, &compare);

	print_list(l);
	
	if (time) {
		times(&t);
		c = clock();
		printf("START: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_start)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - start.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - start.tms_stime)/(double)CLOCKS_PER_SEC );
		printf("LAST: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_last)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - last.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - last.tms_stime)/(double)CLOCKS_PER_SEC );
		last = t;
		real_last = c;
	}
	
	delete_list(l);
	
	if (time) {
		times(&t);
		c = clock();
		printf("START: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_start)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - start.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - start.tms_stime)/(double)CLOCKS_PER_SEC );
		printf("LAST: real: %.10f, user: %.10f, sys: %.10f\n", (double)(c-real_last)/(double)CLOCKS_PER_SEC, (double)(t.tms_utime - last.tms_utime)/(double)CLOCKS_PER_SEC, (double)(t.tms_stime - last.tms_stime)/(double)CLOCKS_PER_SEC );
		last = t;
		real_last = c;
	}
	return 0;
}

