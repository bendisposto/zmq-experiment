#include <zmq.h>
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mqhelper.c"
#include "hashmap.c"
#include "zhelpers.h"

int hashes = 0, work=0, enq = 0;

volatile int next_worker_id = 2;

void *hash_publish, 
*hash_collect, 
*work_publish, 
*work_collect,
*id_response, *queuesizes;

int *queues; 
int q_sizes = 5;

int h_hash (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    zmsg_t *msg = zmsg_recv(hash_collect);	
    if (msg != NULL) {
        //		print_key(string);
        //		printf("\n");	
        int i;
        int size = zmsg_size(msg);
        for (i = 0; i < size; i++) {
            zframe_t *frame = zmsg_pop(msg);
            char *string = malloc(21);
            string = zframe_strdup(frame);
            if(string[0] == 1) put(string); 
            hashes++;
            forward (hash_publish, string);
            free(string);
            zframe_destroy(&frame);
        }
    }
    //free(msg);
    zmsg_destroy(&msg);
    return 0;
}

int h_queues (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {   
    char *string = zstr_recv (queuesizes);
	int worker;
	int qs;
	sscanf(string,"%i %i",&worker,&qs);
//	printf("Worker %s: %s\n",worker,qs);
	queues[worker] = qs;	
	free(string);
	
}

int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {     
    char *string = zstr_recv (work_collect);
    if (string != NULL) {
//         printf("%i,* %s *\n",work,string+21);	
        work++;
        if (!contains(string)) {
            forward_wp(work_publish,string);
            enq++;
            put_local(string);
        }
    }
    free(string);
    return 0;
}

int h_id (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
	char *string = zstr_recv (id_response);
    if (string != NULL) {
        int len = ((int) log10(next_worker_id+1)) + 1;

        char *id = malloc(len + 1);
        sprintf(id, "%d", next_worker_id);

        if (next_worker_id >= q_sizes) {
			q_sizes = q_sizes * 2;
			queues = realloc(queues,q_sizes*sizeof(int));
			int i;
			for (i=q_sizes/2;i<q_sizes;i++) { queues[i] = -42; }
        }
		queues[next_worker_id] = -42;
        
        zmq_msg_t message;
        zmq_msg_init_size (&message, len + 1);
        memcpy (zmq_msg_data (&message), id, len + 1);
        zmq_send (id_response, &message, 0);
        zmq_msg_close (&message);
        
        free(id);
        next_worker_id++;
    }
    free(string);
    return 0;
}

void print_queuesizes() {
	int i;
	for(i=2;i<q_sizes;i++) {
		printf("(%i:%i) ",i,queues[i]);
	}
	printf("\n");
}    

void *print_stats(void *arg) {
    while(1) {
        printf("Hashes: %i/%i Workpackages: %i/%i\n",hashes,count_elements(),work,enq);
		print_queuesizes();
        sleep(5);
    }
    return 0;
}


int main (void)
{
	
	queues = malloc(5*sizeof(int));
	
    pthread_t stats;
    pthread_create (&stats, NULL, print_stats, NULL);
    
    zctx_t *ctx = zctx_new ();
    
    hash_publish = zsocket_new (ctx, ZMQ_PUB);
    hash_collect = zsocket_new (ctx, ZMQ_PULL);
    work_publish = zsocket_new (ctx, ZMQ_PUSH);
    work_collect = zsocket_new (ctx, ZMQ_PULL);
    id_response = zsocket_new (ctx, ZMQ_REP);
    queuesizes = zsocket_new (ctx, ZMQ_PULL);
    
    zsocket_bind (hash_publish, "tcp://*:5000");
    zsocket_bind (hash_collect, "tcp://*:5001");
    zsocket_bind (work_publish, "tcp://*:5002");
    zsocket_bind (work_collect, "tcp://*:5003");
    zsocket_bind (id_response, "tcp://*:5005");
    zsocket_bind (queuesizes, "tcp://*:5006");
    
    printf("le reactor\n");
    zloop_t *reactor = zloop_new ();
    zmq_pollitem_t poller2 = { hash_collect, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller4 = { work_collect, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller6 = { id_response, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller8 = { queuesizes, 0, ZMQ_POLLIN };
    
    zloop_poller (reactor, &poller2, h_hash, NULL);
    zloop_poller (reactor, &poller4, h_work, NULL);
    zloop_poller (reactor, &poller6, h_id, NULL);
    zloop_poller (reactor, &poller8, h_queues, NULL);
    zloop_start  (reactor);
    zloop_destroy (&reactor);
    
    zctx_destroy (&ctx);
    return 0;
}
