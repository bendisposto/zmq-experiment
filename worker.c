#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <czmq.h>
#include "hashmap.c"
#include "queue.c"
#include "sha.c"
#include "graph.c"
#include "zhelpers.h"
#include "mqhelper.c"
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void work_hard();

void  *recv_hashes, *send_hashes, *recv_work, *send_work, *recv_ctrl, *send_ctrl, *recv_tick, *send_tick;
int count = 0;
int hit=0, cache=0;
clock_t start, finish;

volatile int running = 1;

int h_hash(zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
	char *z = NULL;
    	
    while ((z = zstr_recv_nowait(recv_hashes)) != NULL) {
    		printf("H\n");
            put(z);
            free(z); 
	}
 
    return 0; 
}

int h_tick (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
	char *z = s_recv(recv_tick);
	    		printf("R: %s\n",z);
	if (z != NULL) {
		work_hard(); 
	}
	free(z);
	return 0;
}


int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    zmq_recv (recv_work, &message, 0); 
    
    int size = zmq_msg_size (&message);
    char *data = zmq_msg_data (&message);
    char *term = malloc(size-20);
    char *digest = malloc(20);
    memcpy (term, data+20, size-20);
    memcpy (digest, data, 20);  
    
      printf("W: %s S: %i\n",term,size);      
    tCell *new = malloc(sizeof(tCell));
    new->term = term;
    new->digest = digest;
    zmq_msg_close (&message);
    
    enqueue_cell(new);
    return 0;
}

void tick() {
	s_send(send_tick,"TOCK");
}

void work_hard () {
        if (!is_empty()) {     
            tCell *t = dequeue(); 
           
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
                        put_local(d);
                        enqueue(r, d);
                        //send_digest_queued(send_hashes,d);
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
			tick(); 
}


int main (int argc, char *argv []) {
    init_graph();
    
    zctx_t *ctx = zctx_new ();
    
    send_hashes = zsocket_new(ctx, ZMQ_PUSH);
    recv_hashes = zsocket_new(ctx, ZMQ_SUB);
    send_work = zsocket_new(ctx, ZMQ_PUSH);
    send_tick = zsocket_new(ctx, ZMQ_PUSH);
    recv_work = zsocket_new(ctx, ZMQ_PULL);
    recv_tick = zsocket_new(ctx, ZMQ_PULL);
    
    zsocket_connect(send_hashes, "tcp://localhost:5001");
    zsocket_connect(recv_hashes, "tcp://localhost:5000");
    zsocket_connect(send_work, "tcp://localhost:5003");
    zsocket_connect(recv_work, "tcp://localhost:5002");
    int tickport = zsocket_bind(recv_tick, "tcp://*:*");
	char prot[22]; 
	sprintf(prot,"tcp://localhost:%i",tickport);
	printf("%s\n",prot);
	
    zsocket_connect(send_tick, prot);
    
    //	recv_ctrl = zmq_socket (context, ZMQ_REP);
    //	int port = zmq_bind(recv_ctrl, "tcp://*:4567");
    
    //	printf("port: %i\n",port);
    
    
	char *filter = "";
	zmq_setsockopt (recv_hashes, ZMQ_SUBSCRIBE, filter, strlen (filter));
    
    printf("starting\n");
    printf("le reacteur\n");
    zloop_t *reactor = zloop_new ();
    zmq_pollitem_t poller2 = { recv_hashes, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller4 = { recv_work, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller6 = { recv_tick, 0, ZMQ_POLLIN };
    
    zloop_poller (reactor, &poller2, h_hash, NULL);
    zloop_poller (reactor, &poller4, h_work, NULL);
    zloop_poller (reactor, &poller6, h_tick, NULL);
    
    zloop_start  (reactor);
    zloop_destroy (&reactor);
    
    zctx_destroy (&ctx);
    
    
    
    //	enqueue(root,digest);
    // 
    
    /* while (1) {
     *        work_hard();
     *        count++;
}
running = 0;
printf("%d  %d\n",count,count_elements());
printf("Hit: %d Cache: %d\n",hit,cache);
*/
    return 0;
}
