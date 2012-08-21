#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED
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

wQueue *init_queue();
void enqueue_cell(wQueue *q, tCell *cell);
void enqueue(wQueue *q, char *term, char digest[20]);
void enqueue_fifo(wQueue *q, char *term, char digest[20]);
void enqueue_lifo(wQueue *q, char *term, char digest[20]);
tCell *dequeue(wQueue *q);
int is_empty(wQueue *q);
int q_size(wQueue *q);
void print_queue(wQueue *q);

#endif
