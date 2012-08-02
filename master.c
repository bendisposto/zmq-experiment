#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqhelper.c"
#include "hashmap.c"

int messages = 0;

int main (void)
{
    void *context = zmq_init (1);

    void *front = zmq_socket (context, ZMQ_PULL);
    zmq_bind (front, "tcp://*:5557");
    zmq_bind (front, "ipc://hash_collect.ipc");

    void *back = zmq_socket (context, ZMQ_PUB);
    zmq_bind (back, "tcp://*:5556");
    zmq_bind (back, "ipc://hash_distribution.ipc");


    while (1) {
        char *string = recv_digest(front);
		messages++;
		put(string);
        forward (back, string);
		if (messages%1000==0) printf ("%d\n",messages);
    }

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}
