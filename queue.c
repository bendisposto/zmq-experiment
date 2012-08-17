#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Cell {
    char *term;
    char *digest;
    struct Cell* next; 
} tCell;

typedef struct Queue {
    tCell *first;
    tCell *last;
    int c;
} wQueue;

wQueue *init_queue() {
    wQueue *new = malloc(sizeof(wQueue));
    new->first = NULL;
    new->last = NULL;
    new->c = 0;
    return new;
    
}

void enqueue_fifo(wQueue *q, char *term, char digest[20]);

void enqueue_cell(wQueue *q, tCell *cell) {
	q->c++;
    if (q->first == NULL) {
        q->first = cell; 
        q->last = cell;
        q->first->next = NULL;
    }
    else { 
        tCell *temp = q->last; // for multithreading
        q->last = cell;
        temp->next = cell;
    }
}

void enqueue(wQueue *q, char *term, char digest[20]) {
    enqueue_fifo(q, term,digest);
}

void enqueue_fifo(wQueue *q, char *term, char digest[20]){
    q->c++;
    // ignore if in Hashmap 
    // else add to working queue
    tCell *new = malloc(sizeof(tCell));
    new->term = term;
    new->digest = digest;
    new->next = NULL;
    
    //	printf("queued: %s %s\n",term,digest);
    
    
    
    if (q->first == NULL) {
        q->first = new; 
        q->last = new;
        q->first->next = NULL;
    }
    else { 
        tCell *temp = q->last; // for multithreading
        q->last = new;
        temp->next = new;
    }
    
}

void enqueue_lifo(wQueue *q, char *term, char digest[20]) {
    q->c++;
    // ignore if in Hashmap 
    // else add to working queue
    tCell *new = malloc(sizeof(tCell));
    new->term = term;
    new->digest = digest;
    new->next = q->first;
    q->first = new; 
}

tCell *dequeue(wQueue *q) {
    q->c--;
    tCell *res = q->first;
    q->first = q->first->next;
    if (q->first == NULL) { q->last = NULL; }
    res->next = NULL;
    return res;
}

int is_empty(wQueue *q) {
    return (q->first == NULL);
}

int q_size(wQueue *q) {
    return q->c;
}


void print_queue(wQueue *q) {
    printf("Queue: [");
    tCell *p = q->first;
    //	if (first != NULL) printf("%s %i | ", first->term, c);
    while (p != NULL) {
        printf("%s ",p->term);
        //		print_key(p->digest);
        //		printf("\n");
        p = p->next;
    }
    printf("]\n");
}
