#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <czmq.h>
#include "hashmap.c"
//#include "queue.c"
#include "sha.c"
#include "graph.c"
#include "zhelpers.h"
#include "mqhelper.c"
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void work_hard();

void  *recv_hashes, *send_hashes, *recv_work, *send_work, *recv_ctrl, *send_ctrl, *recv_tick, *send_tick, *id_req, *que_info;
char *id;
zloop_t *reactor;

wQueue *local_queue;

int count = 0;
int hit=0, cache=0;
clock_t start, finish;

volatile int running = 1;

int h_hash(zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    char *z = NULL;
    
    while ((z = zstr_recv_nowait(recv_hashes)) != NULL) {
        //        printf("H\n");
        put(z);
        free(z); 
    }
    
    return 0; 
}

int h_tick (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    char *z = s_recv(recv_tick);
    //    printf("R: %s\n",z);
    if (z != NULL) {
        work_hard(); 
    }
    free(z);
    return 0;
}

int h_workrequest (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    // get message from send_work
    // first frame = target
    // second frame = amount
    // dequeue amount workitems
    // send them via send_work
    // 
    zmsg_t *msg = zmsg_recv(send_work);
    zframe_t *target_frame = zmsg_pop(msg);
    zframe_t *amount_frame = zmsg_pop(msg);
    char *target, *a;
    target = zframe_strdup(target_frame);
    s_sendmore(send_work,target);
    free(target);
    
    a = zframe_strdup(amount_frame);
    int amount = atoi(a);  
    
    zmsg_t *wpmsg = zmsg_new();
    for(;amount > 0 && !is_empty(local_queue); amount--) {
        tCell *item = dequeue(local_queue); 
        wp_sendmore(wpmsg,item);
        free(item);
    }
    zmsg_send(&wpmsg, send_work);
    zframe_destroy(&target_frame);
    zframe_destroy(&amount_frame);
    zmsg_destroy(&msg);
    return 0;
}

void tick() {
    s_send(send_tick,"TOCK");
}

int h_work (zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    
    // receive wp message  
    // iterate over frames
    // deserialize content and enque
    // send tock
    zmsg_t *msg = zmsg_recv(recv_work);
    
    int len = zmsg_size(msg);

    int i;
    for (i = 0; i < len; i++) {
        zframe_t *frame = zmsg_pop (msg);
        char *str = zframe_strdup(frame);
        char *digest = malloc(20);
        int len = strlen(str + 20);
        char *term = malloc(len + 1);
        memcpy(term, str + 20, len);
        memcpy(digest, str, 20);
        
        enqueue(local_queue, term, digest);
        
        free(str);
        zframe_destroy(&frame);
    }
    zmsg_destroy(&msg);
    tick();
    
    
    return 0;
}

int h_control(zloop_t *loop, zmq_pollitem_t *poller, void *arg) {
    zmsg_t *msg = zmsg_recv(recv_ctrl);
    char *str = zmsg_popstr (msg);
    zmsg_destroy(&msg);
    if (strcmp(str, "TERM") == 0) {
        free(str);
        return -1;
    }
    free(str);
    return 0;
}

int countRuns = 0;

void work_hard () {
    if (!is_empty(local_queue)) {
        countRuns++;
        tCell *t = dequeue(local_queue); 
        
        assert(t->term != NULL);
        assert(t->digest != NULL);
        
        
        if (!contains_processed(t->digest)) {   
            hit++;
            int l = atoi(t->term);
            zmsg_t *msg = zmsg_new ();
            
            
            int i;
            for (i=0;i<N;i++) {
                if (produce_work(l,i)) { 
                    char *r = malloc(10);
                    sprintf(r,"%d",i);
                    char *d = malloc(20);
                    sha1(r,d);
                    if (!contains(d)) { 
                        put_local(d);
                        enqueue(local_queue, r, d);
                        add_queued_digest(msg, d);
                        //send_digest_queued(send_hashes,d);
                    }
                    else {
                        free(r);
                        free(d);
                    }
                }		
            }
            
            zmsg_send(&msg, send_hashes);
            //		printf("%s done\n",node);
            send_digest_processed(send_hashes, t->digest);
            s_sleep(20);
        }    
        free(t->term);
        free(t->digest);
        free(t);
        
    }
    
    tick(); 
}

char *getId() {
    s_send(id_req, "hello");
    zmsg_t *msg = zmsg_recv(id_req);
    zframe_t *idframe = zmsg_pop(msg);
    zframe_t *modelframe = zmsg_pop(msg);
    char *id = zframe_strdup(idframe);
    char *model = zframe_strdup(modelframe);
    printf("Loading: %s\n", model);
    sleep(2); // pretend to load something
    free(model);
    return id;
}

void *print_stats(void *arg) {
    while(1) {
        int s = q_size(local_queue);
       // printf("Queuesize %i\n", s);
 //       int i, processed = 0;
 //       for (i = 0; i < HASHSIZE; i++) {
   //         if (a[i][20] == 1)
     //           processed++;
       // }

//        printf("Hashes: %d / %d\n", processed, count_elements());
//        printf("work hard runs: %d, processed: %d\n", countRuns, hit);
        if (id != NULL) {

            if (countRuns > 0) {
                int sze = strlen(id) + ((int)log10(s+1))+3;
                //		printf("S: %d\n",sze);
                char *msg = malloc(sze);
                sprintf(msg,"%s %d",id,s);
                s_send(que_info, msg);
                free(msg);
            }
            else {
                int sze = strlen(id) + 4;
                //              printf("S: %d\n",sze);
                char *msg = malloc(sze);
                sprintf(msg,"%s %d", id, -2);
                s_send(que_info, msg);
                free(msg);
            }
        }
        sleep(1);
    }
    
    return 0;
}

int main (int argc, char *argv []) {
    init_graph();
    init_hashmap();
    local_queue = init_queue();
    
    pthread_t stats;
    pthread_create (&stats, NULL, print_stats, NULL);
    
    zctx_t *ctx = zctx_new ();
    
    send_hashes = zsocket_new(ctx, ZMQ_PUSH);
    recv_hashes = zsocket_new(ctx, ZMQ_SUB);
    send_tick = zsocket_new(ctx, ZMQ_PUSH);
    recv_tick = zsocket_new(ctx, ZMQ_PULL);
    recv_ctrl = zsocket_new(ctx, ZMQ_SUB);
    
    id_req = zsocket_new(ctx, ZMQ_REQ);
    zsocket_connect(id_req, "tcp://localhost:5005");
    id = getId();
    printf("my name is %s\n", id);
    
    recv_work = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_set_identity(recv_work,id);
    
    send_work = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_set_identity(send_work,id);
    
    que_info = zsocket_new(ctx, ZMQ_PUSH);
    zsocket_connect(que_info, "tcp://localhost:5006");
    
    zsocket_connect(send_hashes, "tcp://localhost:5001");
    zsocket_connect(recv_hashes, "tcp://localhost:5000");
    zsocket_connect(send_work, "tcp://localhost:5003");
    zsocket_connect(recv_work, "tcp://localhost:5002");
    zsocket_connect(recv_ctrl, "tcp://localhost:5007");
    
    int tickport = zsocket_bind(recv_tick, "tcp://*:*");
    char prot[22]; 
    sprintf(prot,"tcp://localhost:%i",tickport);
    printf("%s\n",prot);
    
    zsocket_connect(send_tick, prot);
    
    
    //	printf("port: %i\n",port);
    
    
    char *filter = "";
    zmq_setsockopt (recv_ctrl, ZMQ_SUBSCRIBE, filter, strlen (filter));
    zmq_setsockopt (recv_hashes, ZMQ_SUBSCRIBE, filter, strlen (filter));
    
    printf("starting\n");
    printf("le reacteur\n");
    reactor = zloop_new ();
    zmq_pollitem_t poller2 = { recv_hashes, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller4 = { recv_work, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller6 = { recv_tick, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller8 = { send_work, 0, ZMQ_POLLIN };
    zmq_pollitem_t poller20 = { recv_ctrl, 0, ZMQ_POLLIN };
    
    zloop_poller (reactor, &poller2, h_hash, NULL);
    zloop_poller (reactor, &poller4, h_work, NULL);
    zloop_poller (reactor, &poller6, h_tick, NULL);
    zloop_poller (reactor, &poller8, h_workrequest, NULL);
    zloop_poller (reactor, &poller20, h_control, NULL);
    
    zloop_start  (reactor);
    zloop_destroy (&reactor);
    
    free(id);
    
    zctx_destroy(&ctx);
    
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
