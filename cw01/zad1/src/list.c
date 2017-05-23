#include <stdlib.h>
#include <string.h>
#include "../include/list.h"

void ala (void) {
	return;
}

static node *node_init(	char *name,
						char *lastname,
						int day,
						int month,
						int year,
						char *email,
						char *phone,
						char *address) {
							
	node *n = calloc(1, sizeof(node));
	
	if (n == NULL)
		return NULL;
	
	size_t len[5] = {0, 0, 0, 0, 0};
	if (name != NULL)
		len[0] = strlen(name);
	if (lastname != NULL)
		len[1] = strlen(lastname);
	if (email != NULL)
		len[2] = strlen(email);
	if (phone != NULL)
		len[3] = strlen(phone);
	if (address != NULL)
		len[4] = strlen(address);
	
	char* buff = malloc(len[0]+len[1]+len[2]+len[3]+len[4]+5);
	
	if (buff == NULL) {
		free(n);
		return NULL;
	}
	
	if (name != NULL)
		n->data.name = strcpy(buff, name);
	if (lastname != NULL)
		n->data.lastname = strcpy(buff+len[0]+1, lastname);
	if (email != NULL)
		n->data.email = strcpy(buff+len[0]+len[1]+2, email);
	if (phone != NULL)
		n->data.phone = strcpy(buff+len[0]+len[1]+len[2]+3, phone);
	if (address != NULL)
		n->data.address = strcpy(buff+len[0]+len[1]+len[2]+len[3]+4, address);
	n->data.day = day;
	n->data.month = month;
	n->data.year = year;
	return n;
}

static void node_destroy(node *n) {
	if (n == NULL)
		return;
	free(n->data.name);
	free(n);
	return;
}

list *create_list() {
	list *l = calloc(sizeof(list), 1);
	return l;
}

void delete_list(list *l) {
	if (l == NULL)
		return;
	node *next;
	while(l->first != NULL) {
		next = l->first->next;
		node_destroy(l->first);
		l->first = next;
	}
	free(l);
	return;
}

void add_node(	list *l,
				char *name,
				char *lastname,
				int day,
				int month,
				int year,
				char *email,
				char *phone,
				char *address) {
	if (l == NULL)
		return;
	node *n = node_init(name, lastname, day, month, year, email, phone, address);
	if (n == NULL)
		return;
	n->prev = NULL;
	n->next = l->first;
	if (n->next != NULL)
		n->next->prev = n;
	l->first = n;
	if (l->last == NULL)
		l->last = l->first;
	return;
}

void delete_node (list *l, node *d) {
	if (d == NULL)
		return;
	
	if (d->next == NULL)
		l->last = d->prev;
	else
		d->next->prev = d->prev;
		
	if (d->prev == NULL)
		l->first = d->next;
	else
		d->prev->next = d->next;
		
	node_destroy(d);
	return;
}

node *find_node (list *l, char *name, char *lastname) {
	
	node *n = l->first;
	while (n != NULL) {
		if (strcmp(name, n->data.name) == 0 && strcmp(lastname, n->data.lastname) == 0)
			return n;
		n = n->next;
	}
	return NULL;
}

static void paste_before (list *l, node *a, node *p) {
	if (l == NULL || a == NULL || p == NULL)
		return;
		
	p->prev = a->prev;
	
	if (a->prev == NULL)
		l->first = p;
	else
		p->prev->next = p;
	
	p->next = a;
	a->prev = p;
	
	return;
}

static void cut (list *l, node *r) {
	if (l == NULL || r == NULL)
		return;
	if (r->next == NULL)
		l->last = r->prev;
	else
		r->next->prev = r->prev;
		
	if (r->prev == NULL)
		l->first = r->next;
	else
		r->prev->next = r->next;
	return;	
}

void sort_list (list *l, int (*compare)(data_t*, data_t*)) {
	
	node *sorted = l->first->next;
	node *i;
	node *tmp;
	
	while(sorted != NULL) {
		tmp = sorted;
		i = l->first;
		while (i != sorted) {
			if (compare(&(tmp->data), &(i->data)) == 1) {
				cut(l, tmp);
				paste_before(l, i, tmp);
				break;
			}
			else
				i = i->next;
		}
		sorted = sorted->next;
	}
	return;
}
