#include "que_utils.h"
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>

int create_queue(key_t key){
    return msgget(key, IPC_CREAT | 0666);
}

int get_queue(key_t key){
    return msgget(key, 0);
}

int delete_queue(int queue){
    return msgctl(queue, IPC_RMID, NULL);
}

void send_msg(int queue, message* msg){
    if ((msgsnd(queue, msg, MESSAGE_SIZE, 0)) == -1){
        perror("Couldn't send message");
    }
}

int receive_msg(int queue, message* msg){
    int received = msgrcv(queue, msg, MESSAGE_SIZE, -100, IPC_NOWAIT);
    if (received == -1){
        // perror("Couldn't receive message");
    }
    return received;
}