#include <zmq.h>
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqhelper.c"
#include "hashmap.c"
#include "zhelpers.h"

int hashes = 0, work=0, enq = 0;

volatile int next_worker_id = 2;

void *hash_publish, 
*hash_collect, 
*work_publish, 
*work_collect;

int h_hash (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    char *string = zstr_recv(hash_collect);	
    if (string != NULL) {
        //		print_key(string);
        //		printf("\n");	
        if(string[0] == 1) put(string);	
        hashes++;
        forward (hash_publish, string);
    }
    free(string);
    return 0;
}

int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {     
    char *string = zstr_recv (work_collect);
    if (string != NULL) {	
        //	printf("%i,* %s *\n",work,string+21);	
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

void *print_stats(void *arg) {
    while(1) {
        printf("Hashes: %i/%i Workpackages: %i/%i\n",hashes,count_elements(),work,enq);
        sleep(5);
    }
    return 0;
}


int main (void)
{
    pthread_t stats;
    pthread_create (&stats, NULL, print_stats, NULL);
    
    
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
