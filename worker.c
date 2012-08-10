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

void *context, *recv_hashes, *send_hashes, *recv_work, *send_work, *recv_ctrl, *send_ctrl;
int count = 0;
int hit=0, cache=0;
clock_t start, finish;

volatile int running = 1;


void *update_hashes(void *arg) {
	while(running) {
		char *z = s_recv(recv_hashes);
		if (z != NULL) {
			if (z[0]<2) {
				put(z);
			}
			else {
				printf("Received Message for Worker %i",z[0]);
			}
		};
		free(z);
	}
	return 0;
}

void *print_stats(void *arg) {
	while(running) {
		printf("Queue: %i\n",q_size());
		sleep(5);
	}
	return 0;
}

void receive_work() {
    zmq_msg_t message;
	zmq_msg_init (&message);
	printf("Waiting for work\n");
	zmq_recv (recv_ctrl, &message, 0); 
    int size = zmq_msg_size (&message);
	char *data = zmq_msg_data (&message);
	printf("continuing\n");

	char *term = malloc(size-20);
	char *digest = malloc(20);
    memcpy (term, data+20, size-20);
    memcpy (digest, data, 20);	
	enqueue(term,digest);
    zmq_msg_close (&message);
    zmq_msg_t reply;
    zmq_msg_init_size (&reply, 3);
    memcpy (zmq_msg_data (&reply), "ok", 3);
    zmq_send (recv_ctrl, &reply, 0);
    zmq_msg_close (&reply);
}


void work_hard() {

	

if (is_empty()) { receive_work(); }
	
assert(!is_empty());
	
	//printf("%d %d %d\n",count,count_elements(),q_size());
	tCell *t = dequeue();

	assert(t->term != NULL);
	assert(t->digest != NULL);
//	printf("process: %s ...",t->term);

	
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
//				 put_local(d);
				 enqueue(r,d); 
			 	 send_digest_queued(send_hashes,d);
				}
				else {
					free(r);
					free(d);
				}
			}		
		}
//		printf("%s done\n",node);
		send_digest_processed(send_hashes, t->digest);
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
	
	send_hashes = zmq_socket (context, ZMQ_PUSH);

    zmq_connect (send_hashes, "tcp://localhost:5557");

    recv_hashes = zmq_socket (context, ZMQ_SUB);
    zmq_connect (recv_hashes, "tcp://localhost:5556");

	recv_ctrl = zmq_socket (context, ZMQ_REP);
	int port = zmq_bind(recv_ctrl, "tcp://*:4567");
	
	printf("port: %i\n",port);


	char *filter = "";
	zmq_setsockopt (recv_hashes, ZMQ_SUBSCRIBE, filter, strlen (filter));

    pthread_t worker;
    pthread_create (&worker, NULL, update_hashes, (void*) &context);

    pthread_t stats;
    pthread_create (&stats, NULL, print_stats, NULL);

    
	printf("starting\n");
	



//	enqueue(root,digest);
// 
   
	while (1) {
	   work_hard();
		count++;
    }
	running = 0;
	printf("%d  %d\n",count,count_elements());
	printf("Hit: %d Cache: %d\n",hit,cache);
	
	
    zmq_close (send_hashes);
    zmq_close (recv_hashes);
    zmq_term (context);

    return 0;
}
