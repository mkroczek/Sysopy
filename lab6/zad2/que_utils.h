#ifndef QUE_H
#define QUE_H
#include <sys/ipc.h>
#include <mqueue.h>
#include "constants.h"

// typedef struct message{
//     long type;
//     int id;
//     char text[MAX_MSG_LEN];
// }message;

int create_queue(char* queue_name);
int get_queue(char* queue_name);
int delete_queue(char* queue_name);
int close_queue(mqd_t mqdes);
int send_msg(mqd_t mqdes, char* msg, int type);
int receive_msg(mqd_t mqdes, char* msg, int* type);

#endif