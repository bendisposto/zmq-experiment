#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "mqhelper.c"

void print_key(char key[20]) {
	int o; for(o=0;o<20;o++) printf("%d ",key[o]);
}


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
//		print_key(string);
//		printf(" %s\n",string);
        send_digest (back, string);

    }

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}