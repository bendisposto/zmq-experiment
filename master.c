#include <zmq.h>
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqhelper.c"
#include "hashmap.c"
#include "zhelpers.h"

int messages = 0;

volatile int next_worker_id = 2;

void *hash_publish, 
     *hash_collect, 
     *work_publish, 
	*work_collect;

int h_hash (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
	char *string = zstr_recv(hash_collect);	
	if (string != NULL) {
	messages++;
    forward (hash_publish, string);
	if (messages%1000==0) printf ("%d\n",messages);
}
	free(string);
}

int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {     
	char *string = zstr_recv_nowait (work_collect);	
	printf("Tralala der %s is da.\n",string);
	free(string);
}

int main (void)
{
     zctx_t *ctx = zctx_new ();

    hash_publish = zsocket_new (ctx, ZMQ_PUB);
    hash_collect = zsocket_new (ctx, ZMQ_PULL);
    work_publish = zsocket_new (ctx, ZMQ_PUSH);
    work_collect = zsocket_new (ctx, ZMQ_PULL);
     
    zsocket_bind (hash_publish, "tcp://*:5000");
    zsocket_bind (hash_collect, "tcp://*:5001");
    zsocket_bind (work_publish, "tcp://*:5002");
    zsocket_bind (work_collect, "tcp://*:5003");

 
	printf("le reactor\n");
   zloop_t *reactor = zloop_new ();
   zmq_pollitem_t poller2 = { hash_collect, 0, ZMQ_POLLIN };
   zmq_pollitem_t poller4 = { work_collect, 0, ZMQ_POLLIN };

   zloop_poller (reactor, &poller2, h_hash, NULL);
   zloop_poller (reactor, &poller4, h_work, NULL);
   zloop_start  (reactor);
   zloop_destroy (&reactor);

	zctx_destroy (&ctx);
    return 0;
}
