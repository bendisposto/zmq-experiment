#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"
#include "hashmap.c"
#include "queue.c"
#include "sha.c"
#include "graph.c"
#include <pthread.h>
#include "mqhelper.c"

void *context, *front, *back;

int main (int argc, char *argv []) {

	init_graph();
	context = zmq_init (1);
	

    back = zmq_socket (context, ZMQ_PUSH);

    zmq_connect (back, "tcp://localhost:5557");
 //   zmq_bind (back, "ipc://hash_distribution.ipc");

	char digest[20];
    sha1("I am legend!",digest);
	send_digest(back,digest);

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}