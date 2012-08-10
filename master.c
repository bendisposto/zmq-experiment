#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqhelper.c"
#include "hashmap.c"
#include "zhelpers.h"

int messages = 0;

volatile int next_worker_id = 2;

void *id_requests(void *arg) {
		while(1) {
			char *z = s_recv(id_requests);
			
			free(z);
		}
}

int main (void)
{
    void *context = zmq_init (1);

    void *hash_publish = zmq_socket (context, ZMQ_PUB);
    zmq_bind (hash_publish, "tcp://*:5556");
    zmq_bind (hash_publish, "ipc://hash_publish.ipc");  

    void *hash_collect = zmq_socket (context, ZMQ_PULL);
    zmq_bind (hash_collect, "tcp://*:5557");
    zmq_bind (hash_collect, "ipc://hash_collect.ipc");

    void *id_requests = zmq_socket (context, ZMQ_REP);
    zmq_bind (id_requests, "tcp://*:5558");
    zmq_bind (id_requests, "ipc://id_requests.ipc");


    while (1) {
        char *string = s_recv(hash_collect);
		messages++;
		put(string);
        forward (hash_publish, string);
		if (messages%1000==0) printf ("%d\n",messages);
    }

    zmq_close (hash_publish);
    zmq_close (hash_collect);
    zmq_term (context);
    return 0;
}
