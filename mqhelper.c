#include <zmq.h>
#include <stdio.h>
#include <string.h>

int send_digest_processed(void *socket, char string[20]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21);
		memcpy (zmq_msg_data (&message), "\1", 1);
	    memcpy (zmq_msg_data (&message)+1, string, 20);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}
int send_digest_queued(void *socket, char string[20]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21);
		memcpy (zmq_msg_data (&message), "\0", 1);
	    memcpy (zmq_msg_data (&message)+1, string, 20);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}

int forward(void *socket, char string[21]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21);
	    memcpy (zmq_msg_data (&message), string, 21);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}
