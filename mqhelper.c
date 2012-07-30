#include <zmq.h>
#include <stdio.h>
#include <string.h>

int send_digest_processed(void *socket, char string[20]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21);
	    memcpy (zmq_msg_data (&message), string, 20);
		memcpy (zmq_msg_data (&message)+20, "\1", 1);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}
int send_digest_queued(void *socket, char string[20]) {
	    int rc;
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21);
	    memcpy (zmq_msg_data (&message), string, 20);
		memcpy (zmq_msg_data (&message)+20, "\0", 1);
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

char *recv_digest (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, 0))
        return (NULL);
    char *string = malloc (21);
    memcpy (string, zmq_msg_data (&message), 21);
    zmq_msg_close (&message);
    return (string);
}

static char *recv_digest_async (void *socket) {
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, ZMQ_NOBLOCK))
        return (NULL);
    char *string = malloc (21);
    memcpy (string, zmq_msg_data (&message), 21);
    zmq_msg_close (&message);
    return (string);
}