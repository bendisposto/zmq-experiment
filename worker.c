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
	printf("le receiver\n");
	while(running) {
		char *z = s_recv(recv_hashes);
//		printf("R: %s\n",z);
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



tCell *receive_work() {
    zmq_msg_t message;
	zmq_msg_init (&message);
	zmq_recv (recv_work, &message, 0); 
	
    int size = zmq_msg_size (&message);
	char *data = zmq_msg_data (&message);
	char *term = malloc(size-20);
	char *digest = malloc(20);
    memcpy (term, data+20, size-20);
    memcpy (digest, data, 20);	

//	printf("W: %s S: %i\n",term,size);	
	tCell *new = malloc(sizeof(tCell));
	new->term = term;
	new->digest = digest;
	
    zmq_msg_close (&message);
	return new;
}


void work_hard() {

	tCell *t = receive_work(); 
	s_sleep(10);

	assert(t->term != NULL);
	assert(t->digest != NULL);

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
				   zmq_msg_t message;
				   zmq_msg_init_size (&message, 20+strlen(r));
				   memcpy (zmq_msg_data (&message), d, 20);
				   memcpy (zmq_msg_data (&message)+20, r, strlen(r));
   			       zmq_send (send_work, &message, 0);
				   zmq_msg_close (&message);
				   put_local(d);

//			 	   send_digest_queued(send_hashes,d);
				}
				else {
					free(r);
					free(d);
				}
			}		
		}
//		printf("%s done\n",node);
		send_digest_processed(send_hashes, t->digest);
	
	
	free(t->term);
	free(t->digest);
	free(t);
}


int main (int argc, char *argv []) {
	init_graph();

	context = zmq_init (1); 
	
	send_hashes = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (send_hashes, "tcp://localhost:5001");

    recv_hashes = zmq_socket (context, ZMQ_SUB);
    zmq_connect (recv_hashes, "tcp://localhost:5000");

	send_work = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (send_work, "tcp://localhost:5003");

    recv_work = zmq_socket (context, ZMQ_PULL);
    zmq_connect (recv_work, "tcp://localhost:5002");



//	recv_ctrl = zmq_socket (context, ZMQ_REP);
//	int port = zmq_bind(recv_ctrl, "tcp://*:4567");
	
//	printf("port: %i\n",port);


	char *filter = "";
	zmq_setsockopt (recv_hashes, ZMQ_SUBSCRIBE, filter, strlen (filter));

    pthread_t worker;
    pthread_create (&worker, NULL, update_hashes, (void*) &context);

//    pthread_t stats;
//    pthread_create (&stats, NULL, print_stats, NULL);

    
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
