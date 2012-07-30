#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"
#include "hashmap.c"
#include "queue.c"
#include "sha.c"
#include "graph.c"

void *context, *front, *back;
int count = 0;

void found_new_hash(char* digest) {
	put(digest);
	s_send (back, digest);
}

void work_hard() {
//	print_queue();
	char *node = dequeue();
	
	char digest[20];
    sha1(node,digest);
	
	if (!contains(digest)) {
		// call Prolog code that produces sucessors	
		printf("process %s\n",node);
		int i;
		for (i=0;i<N;i++) {
			char *r = produce_work(node,i);
			//if (r) printf("enqueue: %s\n",r);
			if (r) enqueue(r);		
		}
		found_new_hash(digest);
	}
	else {
		printf("cache hit %s\n",node);
	}
//	print_queue();
}


int main (int argc, char *argv []) {

	init_graph();
	context = zmq_init (1);
    front = zmq_socket (context, ZMQ_SUB);
    back = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (front, "tcp://localhost:5556");
 //   zmq_bind (front, "ipc://hash_collect.ipc");

    zmq_connect (back, "tcp://localhost:5557");
 //   zmq_bind (back, "ipc://hash_distribution.ipc");

	enqueue("some_work_package 0");
	while (!is_empty()) {
	   work_hard();
    }

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}