#ifndef MQHELPER_H_INCLUDED
#include "queue.h"
#define MQHELPER_H_INCLUDED

char *int2string(int i);
int send_digest_processed(void *socket, char string[20]);
int send_digest_queued(void *socket, char string[20]);
int add_queued_digest(zmsg_t *msg, char string[20]);
int forward(void *socket, char string[21]);
int forward_wp(void *socket, char *string);
int wp_sendmore(zmsg_t *msg, tCell *wp);

#endif
