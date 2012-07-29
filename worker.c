#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include "zhelpers.h"
#include "hashmap.c"
#include "queue.c"
#include "sha.c"
#include "graph.c"

void *context, *front, *back;
int count = 0;

void work_hard() {
//	print_queue();
	char *node = dequeue();
	
	char digest[20];
    sha1(node,digest);
	
	if (!contains(digest)) {
		// call Prolog code that produces sucessors	
		printf("process %s\n",node);
		int i;
		for (i=0;i<N;i++) {
			char *r = produce_work(node,i);
			//if (r) printf("enqueue: %s\n",r);
			if (r) enqueue(r);		
		}

		put(digest);
	}
	else {
		printf("cache hit %s\n",node);
	}
//	print_queue();
}


int main (int argc, char *argv [])
{
/*    char digest[20];
    sha1("1234567890",digest);
    int o;
    for (o=0; o<20; o++) {
        printf("%i ",digest[o]);
    }
	printf("\n"); */
	init_graph();
	enqueue("some_work_package 0");
	printf("%i\n",is_empty());
	while (!is_empty()) {
	   work_hard();
    }
	exit(-4);

	context = zmq_init (1);
    front = zmq_socket (context, ZMQ_SUB);
    back = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (front, "tcp://localhost:5556");
 //   zmq_bind (front, "ipc://hash_collect.ipc");

    zmq_connect (back, "tcp://localhost:5557");
 //   zmq_bind (back, "ipc://hash_distribution.ipc");

    char *text =  (argc > 1)? argv [1]: "apfelkompott";

	enqueue(text);
	enqueue(text);
	print_queue();
    
    work_hard();

	print_queue();

    zmq_close (back);
    zmq_close (front);
    zmq_term (context);
    return 0;
}