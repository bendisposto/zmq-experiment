#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.c"
#include "queue.c"
#include "sha.c"
#include "graph.c"
#include "zhelpers.h"
#include "mqhelper.c"
#include <unistd.h>
#include <pthread.h>

void *context, *front, *back;
int count = 0;
int hit=0, cache=0;


void update_hashes() {
	while(1) {
		char *z = recv_digest(front);
//		printf("%i ",z[20]);
		if (z != NULL) put(z);
	}
}

void work_hard() {

//	printf("%d %d %d\n",count,count_elements(),q_size());
	char *node = "";
	char *digest = "";
	dequeue(&node,&digest);

//	printf("process: %s ...",node);

	
	if (!contains_processed(digest)) {
		hit++;
		int i;

		int l = atoi(node);

		for (i=0;i<N;i++) {
			char *r = produce_work(l,i);

			if (r) { 
				char *d = malloc(20);
			    sha1(r,d);
			    if (!contains(d)) { 
				 enqueue(r,d); 
			 	 send_digest_queued(back,d);
				}
			}		
		}
//		printf("done\n");
		send_digest_processed(back, digest);
	}
	else {
		cache++;
//		printf("cache hit\n");
	}
//	printf("%d\n",q_size());
//	print_queue();
}


int main (int argc, char *argv []) {

	init_graph();
	context = zmq_init (1);
	

    back = zmq_socket (context, ZMQ_PUSH);

//    zmq_connect (back, "tcp://localhost:5557");
    zmq_connect (back, "ipc://hash_collect.ipc");

 //   zmq_bind (back, "ipc://hash_distribution.ipc");

    front = zmq_socket (context, ZMQ_SUB);
//    zmq_connect (front, "tcp://localhost:5556");
    zmq_connect (front, "ipc://hash_distribution.ipc");

	char *filter = "";
	zmq_setsockopt (front, ZMQ_SUBSCRIBE, filter, strlen (filter));

    pthread_t worker;
    int rc = pthread_create (&worker, NULL, update_hashes, (void*) &context);


	printf("starting\n");
	

	char *root = "0";	
	char digest[20];
    sha1(root,digest);

	enqueue(root,digest);

	while (!is_empty()) {
	   work_hard();
		count++;
    }
	printf("%d  %d\n",count,count_elements());
	printf("Hit: %d Cache: %d\n",hit,cache);

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}