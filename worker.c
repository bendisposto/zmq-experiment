#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"

int main (int argc, char *argv [])
{
    void *context = zmq_init (1);

 //   void *front = zmq_socket (context, ZMQ_SUB);
 //   zmq_connect (front, "tcp://localhost:5556");
 //   zmq_bind (front, "ipc://hash_collect.ipc");

    void *back = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (back, "tcp://localhost:5557");
 //   zmq_bind (back, "ipc://hash_distribution.ipc");


	char *text =  (argc > 1)? argv [1]: "apfelkompott";
    s_send (back, text);
    

    zmq_close (back);
  //  zmq_close (front);
    zmq_term (context);
    return 0;
}