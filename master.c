#include <zmq.h>
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mqhelper.c"
//#include "hashmap.c"
#include "zhelpers.h"

#define NOT_INITIALIZED	-1
#define MINIMUM_SIZE  200
#define FIRST_WORKER	2
#define N_WORKERS   16

int hashes = 0, work=0, enq = 0;

volatile int next_worker_id = FIRST_WORKER;

void *hash_publish, 
*hash_collect, 
*work_publish, 
*work_collect,
*id_response,
*queuesizes,
*send_ctrl,
*recv_ctrl;

int queues[N_WORKERS]; 

int checkShutdown();


int h_hash (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    zmsg_t *msg = zmsg_recv(hash_collect);	
    if (msg != NULL) {
        zmsg_send(&msg, hash_publish);
    }
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
    if (checkShutdown())
        return -1;
    return 0;
}

int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    work++;
    // Receive and forward
    zmsg_t *msg = zmsg_recv(work_collect);
    zframe_t *nullframe = zmsg_pop (msg);
    zframe_destroy(&nullframe);
    if (msg != NULL) {
        zmsg_send(&msg, work_publish);       
    }
    zmsg_destroy(&msg);
    return 0;
}

int h_id (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    char *string = zstr_recv (id_response);
    if (string != NULL) {
        int len = ((int) log10(next_worker_id+1)) + 1;
        
        if (next_worker_id >= N_WORKERS) {
            printf("We only support %i workers.",N_WORKERS);
            return -1;
        }
        
        char *id = malloc(len + 1);
        sprintf(id, "%d", next_worker_id);
        queues[next_worker_id] = NOT_INITIALIZED;
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
    for(i = 2; i < N_WORKERS; i++) {
        printf("(%i:%i) ",i,queues[i]);
    }
    printf("\n");
}    

void transfer_work(int from, int to, int amount) {
    printf("I demand that %i sends %i workpackages to %i\n", from, amount,to);
    char *sfrom, *sto, *samount;
    
    sfrom = int2string(from);
    sto = int2string(to);
    samount = int2string(amount);
    
    s_sendmore(work_collect,sfrom);
    s_sendmore(work_collect, sto);
    s_send(work_collect, samount);
    
    free(sto);
    free(sfrom);
    free(samount);
}

int checkShutdown() {
    if (work == 0)
        return 0;
    
    int shutdown = 1;
    int started = 0;
    int i;
    for (i = 0; i < N_WORKERS; i++) {
        if (queues[i] == 0) {
            started = 1;
        }
        if (queues[i] > 0) {
            shutdown = 0;
            break;
        }
    }
    return shutdown && started;
}

void *print_stats(void *arg) {
    while(1) {
        printf("Workpackages: %i/%i\n",work,enq);
        print_queuesizes();
        if (next_worker_id - FIRST_WORKER > 1) {
            int need = 0, has = 0;
            int i; for(i = FIRST_WORKER; i<next_worker_id; i++) {
                if (queues[i]>queues[has]) has = i;
           if (queues[i] == 0 || queues[i] == -2) need = i;
           if (queues[has] > MINIMUM_SIZE*2 && need > 0) { transfer_work(has,need,queues[has]/2); break; }
            } 
        }
        sleep(1);
    }
    return 0;
}


int main (void)
{
    
    //queues = malloc(5*sizeof(int));
    int i;
    for (i = 0; i < N_WORKERS; i++) {
        queues[i] =  NOT_INITIALIZED;
    }
    
    pthread_t stats;
    pthread_create (&stats, NULL, print_stats, NULL);
    
    zctx_t *ctx = zctx_new ();
    
    hash_publish = zsocket_new (ctx, ZMQ_PUB);
    hash_collect = zsocket_new (ctx, ZMQ_PULL);
    work_publish = zsocket_new (ctx, ZMQ_ROUTER);
    work_collect = zsocket_new (ctx, ZMQ_ROUTER);
    id_response = zsocket_new (ctx, ZMQ_REP);
    queuesizes = zsocket_new (ctx, ZMQ_PULL);
    send_ctrl = zsocket_new (ctx, ZMQ_PUB);
    recv_ctrl = zsocket_new(ctx, ZMQ_SUB);

    zsocket_bind (hash_publish, "tcp://*:5000");
    zsocket_bind (hash_collect, "tcp://*:5001");
    zsocket_bind (work_publish, "tcp://*:5002");
    zsocket_bind (work_collect, "tcp://*:5003");
    zsocket_bind (id_response, "tcp://*:5005");
    zsocket_bind (queuesizes, "tcp://*:5006");
    zsocket_bind (send_ctrl, "tcp://*:5007");
    
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
    s_send(send_ctrl, "TERM");
    sleep(1);
    zloop_destroy (&reactor);
    
    zctx_destroy (&ctx);
    return 0;
}
