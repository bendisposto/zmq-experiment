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
#include <time.h>

void *context, *front, *back;
int count = 0;
int hit=0, cache=0;
clock_t start, finish;


void *update_hashes(void *arg) {
	while(1) {
		char *z = recv_digest(front);
//		printf("%i ",z[20]);
		if (z != NULL) put(z);
		free(z);
	}
}

void *print_stats(void *arg) {
	while(1) {
		printf("Queue: %i\n",q_size());
		sleep(5);
	}
}


void work_hard() {

	//printf("%d %d %d\n",count,count_elements(),q_size());
	tCell *t = dequeue();

	assert(t->term != NULL);
	assert(t->digest != NULL);
//	printf("process: %s ...",node);

	
	if (!contains_processed(t->digest)) {
		hit++;
		int i;

		int l = atoi(t->term);

		for (i=0;i<N;i++) {
			if (produce_work(l,i)) { 
				char *r = malloc(10);
				sprintf(r,"%d",i);
				char *d = malloc(20);
			    sha1(r,d);
			    if (!contains(d)) { 
				 enqueue(r,d); 
			 	 send_digest_queued(back,d);
				}
				else {
					free(r);
					free(d);
				}
			}		
		}
//		printf("%s done\n",node);
		send_digest_processed(back, t->digest);
	}
	else {
		cache++;
//		printf("cache hit\n");
	}
	
	free(t->term);
	free(t->digest);
	free(t);

	
//	printf("%d\n",q_size());
//	print_queue();
}


int main (int argc, char *argv []) {


	init_graph();
	

	context = zmq_init (1); 
	
	back = zmq_socket (context, ZMQ_PUSH);

    zmq_connect (back, "tcp://localhost:5557");
//    zmq_connect (back, "ipc://hash_collect.ipc");

 //   zmq_bind (back, "ipc://hash_distribution.ipc");

    front = zmq_socket (context, ZMQ_SUB);
    zmq_connect (front, "tcp://localhost:5556");
//    zmq_connect (front, "ipc://hash_distribution.ipc");

	char *filter = "";
	zmq_setsockopt (front, ZMQ_SUBSCRIBE, filter, strlen (filter));

    pthread_t worker;
    int rc = pthread_create (&worker, NULL, update_hashes, (void*) &context);

    pthread_t stats;
    int sc = pthread_create (&worker, NULL, print_stats, NULL);

    
	printf("starting\n");
	

	char *root = malloc(2);
	memcpy(root,"0",2);
	
	if (argc > 1) root = argv[1];
	
	
		
	char *digest = malloc(20);
    sha1(root,digest);

	enqueue(root,digest);

	while (!is_empty()) {

	   work_hard();

		count++;
    }
	printf("%d  %d\n",count,count_elements());
	printf("Hit: %d Cache: %d\n",hit,cache);
//	sleep(2); // Allow receiver to get all messages
		
    zmq_close (back);
    zmq_close (front);
    zmq_term (context);

    return 0;
}
