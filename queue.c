#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Cell {
  char *term;
  char *digest;
  struct Cell* next; 
} tCell;

tCell *first=NULL, *last=NULL;

int c = 0;

void enqueue_fifo(char *term, char digest[20]);


void enqueue(char *term, char digest[20]) {
	enqueue_fifo(term,digest);
}

void enqueue_fifo(char *term, char digest[20]){
	c++;
	// ignore if in Hashmap 
	// else add to working queue
	tCell *new = malloc(sizeof(tCell));
	new->term = term;
	new->digest = digest;
	new->next = NULL;
	
//	printf("queued: %s %s\n",term,digest);
	
	if (first == NULL)  first = new; 
	else 
		last->next = new;
		last = new;
}

void enqueue_lifo(char *term, char digest[20]) {
	c++;
	// ignore if in Hashmap 
	// else add to working queue
	tCell *new = malloc(sizeof(tCell));
	new->term = term;
	new->digest = digest;
	new->next = first;
	first = new; 
}


tCell *dequeue() {
	c--;
	tCell *res = first;
	first = first->next;
	res->next = NULL;
	return res;
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
//	if (first != NULL) printf("%s %i | ", first->term, c);
	while (p != NULL) {
		printf("%s ",p->term);
//		print_key(p->digest);
//		printf("\n");
		p = p->next;
	}
	printf("]\n");
}
