#ifndef QUE_H
#define QUE_H
#include <sys/ipc.h>
#include "constants.h"

typedef struct message{
    long type;
    int id;
    char text[MAX_MSG_LEN];
}message;

int create_queue(key_t key);
int get_queue(key_t key);
int delete_queue(int queue);
void send_msg(int queue, message* msg);
int receive_msg(int queue, message* msg);

#define MESSAGE_SIZE sizeof(message) - sizeof(long)

#endif