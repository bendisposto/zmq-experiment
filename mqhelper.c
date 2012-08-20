#include <zmq.h>
#include <czmq.h>
#include <stdio.h>
#include <string.h>


char *int2string(int i) {
	int len = (int)log10(i) + 2;
	char *res = malloc(len);
	sprintf(res,"%i",i);
	return res;
}

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

// requires zmsg_t *msg = zmsg_new (); beforehand
int add_queued_digest(zmsg_t *msg, char string[20]) {
    char tmp[21];
    memcpy (tmp, "\0", 1);
    memcpy (tmp+1, string, 20);
    zmsg_addmem(msg, tmp, 21);
    return 0;
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


int forward_wp(void *socket, char *string) {
	    int rc;
		int len = strlen(string+20);
	    zmq_msg_t message;
	    zmq_msg_init_size (&message, 21+len);
	    memcpy (zmq_msg_data (&message), string, 20);
	    memcpy (zmq_msg_data (&message)+20, string+20, len);
		memcpy (zmq_msg_data (&message)+20+len, "\0", 1);
	    rc = zmq_send (socket, &message, 0);
	    zmq_msg_close (&message);
	    return (rc);
}
