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

	context = zmq_init (1);
    back = zmq_socket (context, ZMQ_REQ);

    zmq_connect (back, "tcp://localhost:4567");
		
	char *digest = malloc(20);
	
    sha1("0",digest);

    zmq_msg_t message;
    zmq_msg_init_size (&message, 22);
    memcpy (zmq_msg_data (&message), digest, 20);
    memcpy (zmq_msg_data (&message)+20, "0", 2);

    zmq_send (back, &message, 0);
    zmq_msg_close (&message);

    zmq_close (recv_hashes);
    zmq_term (context);

    return 0;
}