#include <zmq.h>
#include <stdio.h>
#include <string.h>

int send_digest(void *socket, char string[20]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 20);
	    memcpy (zmq_msg_data (&message), string, 20);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}

char *recv_digest (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, 0))
        return (NULL);
    char *string = malloc (20);
    memcpy (string, zmq_msg_data (&message), 20);
    zmq_msg_close (&message);
    return (string);
}

static char *recv_digest_async (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, ZMQ_NOBLOCK))
        return (NULL);
    char *string = malloc (20);
    memcpy (string, zmq_msg_data (&message), 20);
    zmq_msg_close (&message);
    return (string);
}