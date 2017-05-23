#ifndef _LIST_H
#define _LIST_H

typedef struct {
	char *name;
	char *lastname;
	int day;
	int month;
	int year;
	char *email;
	char *phone;
	char *address;
} data_t;

struct node_t {
	struct node_t *next;
	struct node_t *prev;
	data_t data;
};

typedef struct node_t node;

typedef struct {
	node *first;
	node *last;
} list;

#ifndef DLL_BUILD
list *create_list();

void delete_list(list *l);
void add_node(	list *l,
				char *name,
				char *lastname,
				int day,
				int month,
				int year,
				char *email,
				char *phone,
				char *address);
void delete_node (list *l, node *d);
node *find_node (list *l, char *name, char *lastname);
void sort_list (list *l, int (*compare)(data_t*, data_t*));
#endif
#endif
