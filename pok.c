#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"
#include "hashmap.c"
//#include "queue.c"
#include "sha.c"
#include "graph.c"
#include <pthread.h>
#include "mqhelper.c"
#include <czmq.h>

void *context, *front, *back;
char *get_root(int argc, char *argv []);

int user_main (int argc, char *argv []) {
    
    context = zmq_init (1);
    back = zmq_socket (context, ZMQ_DEALER);
    
    zmq_connect (back, "tcp://localhost:5003");
    
    char *digest = malloc(20);
    char *root = get_root(argc, argv);
    sha1(root,digest);

    
    /*zmq_msg_t message;
    zmq_msg_init_size (&message, 22);
    
    memcpy (zmq_msg_data (&message), digest, 20);
    memcpy (zmq_msg_data (&message)+20, "0", 2);
    
    zmq_send (back, &message, 0);
    zmq_msg_close (&message);*/
    char wp[22];
    memcpy(wp, digest, 20);
    memcpy(wp + 20, root, 2);
    zmsg_t *message = zmsg_new();  
    zmsg_addmem(message, wp, 22);
    zmsg_pushstr(message, "2");
    zmsg_send(&message, back);
    
    zmq_msg_t message2;
    front = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (front, "tcp://localhost:49152");
    
    zmq_msg_init_size (&message2, 5);
    memcpy (zmq_msg_data (&message2), "TOCK", 5);
    zmq_send (front, &message2, 0);
    zmq_msg_close (&message2);
    
    free(root);
    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    
    return 0;
}


#include <stdio.h>
#include <sicstus/sicstus.h>

char *get_root(int argc, char *argv [])
{
    int rval;
    SP_pred_ref pred;
    SP_qid goal;
    SP_term_ref path;
    
    /* Initialize Prolog engine. The third arg to SP_initialize is
     *        an option block and can be NULL, for default options. */
    if (SP_FAILURE == SP_initialize(argc, argv, NULL)) {
        fprintf(stderr, "SP_initialize failed: %s\n",
                SP_error_message(SP_errno));
        exit(1);
    }
    
    rval = SP_restore("root.sav");
    
    if (rval == SP_ERROR || rval == SP_FAILURE) {
        fprintf(stderr, "Could not restore \"root.sav\".\n");
        exit(1);
    }
    
    /* train.c */
    
    /* Look up bar/1. */
    if (!(pred = SP_predicate("root",1,"user"))) {
        fprintf(stderr, "Could not find root/1.\n");
        exit(1);
    }
    
    /* Create the three arguments to connected/4. */
    SP_put_variable(path = SP_new_term_ref());
    
    /* Open the query. In a development system, the query would look like:
     * 
     * | ?- bar(X).
     */
    if (!(goal = SP_open_query(pred,path))) {
        fprintf(stderr, "Failed to open query.\n");
        exit(1);
    }
    
    /*
     * Loop through all the solutions.
     */
    char *root;
    while (SP_next_solution(goal)==SP_SUCCESS)
    {
        printf("Root: ");
        
        const char *text;
        SP_term_ref via = SP_new_term_ref();
        
        SP_put_term(via,path);    
        SP_get_string(via, &text);
        root = malloc(strlen(text) + 1);
        memcpy(root, text, strlen(text) + 1);
        printf("%s",text);
        
        printf("\n");
    }
    
    SP_close_query(goal);
    
    return root;
}