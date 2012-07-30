#include <stdio.h>
#include <string.h>

typedef struct Cell {
  char *term;
  struct Cell* next; 
} tCell;

tCell *first=NULL, *last=NULL;

int c = 0;

void enqueue(char *addr) {
	c++;
	// ignore if in Hashmap 
	// else add to working queue
	tCell *new = malloc(sizeof(tCell));
	new->term = addr;
	new->next = NULL;
	
	if (first == NULL)  first = new; 
	else 
		last->next = new;
		last = new;
}

char *dequeue() {
	c--;
	char *term = first->term;
	tCell *next = first->next;
	free(first);
	first = next;
	return term;
}

int is_empty() {
	return (first == NULL);
}

int q_size() {
	return c;
}


void print_queue() {
	printf("Queue: [");
	tCell *p = first;
	while (p != NULL) {
		printf("%s ",p->term);
		p = p->next;
	}
	printf("]\n");
}