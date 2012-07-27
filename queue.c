#include <stdio.h>
#include <string.h>

typedef struct Cell {
  char *term;
  struct Cell* next; 
} tCell;

tCell *first=NULL, *last=NULL;

void enqueue(char *addr) {
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
	char *term = first->term;
	tCell *next = first->next;
	free(first);
	first = next;
	return term;
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