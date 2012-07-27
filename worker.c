#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"

void *context, *front, *back;
int count = 0;

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


void work_hard() {
	// take first from queue
	printf("D: %s\n",dequeue());
	printf("D: %s\n",dequeue());
	// sync hashmap 
	// call Prolog code that produces sucessors	
	sleep(1);
}


int main (int argc, char *argv [])
{
    
	context = zmq_init (1);
    front = zmq_socket (context, ZMQ_SUB);
    back = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (front, "tcp://localhost:5556");
 //   zmq_bind (front, "ipc://hash_collect.ipc");

    zmq_connect (back, "tcp://localhost:5557");
 //   zmq_bind (back, "ipc://hash_distribution.ipc");

    char *text =  (argc > 1)? argv [1]: "apfelkompott";

	enqueue(text);
	enqueue(text);
	print_queue();
    
   work_hard();

	print_queue();

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}