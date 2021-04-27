#include "que_utils.h"
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

mqd_t create_queue(char* queue_name){
    struct mq_attr attr;
    attr.mq_flags = O_NONBLOCK;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_LEN-1;
    attr.mq_curmsgs = 0;

    return mq_open(queue_name, O_RDONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0666, &attr);
}

int get_queue(char* queue_name){
    return mq_open(queue_name, O_WRONLY | O_NONBLOCK);
}

int delete_queue(char* queue_name){
    return mq_unlink(queue_name);
}

int close_queue(mqd_t mqdes){
    return mq_close(mqdes);
}

int send_msg(mqd_t mqdes, char* msg, int type){
    return mq_send(mqdes, msg, strlen(msg), type);
}

int receive_msg(mqd_t mqdes, char* msg, int* type){
    return mq_receive(mqdes, msg, MAX_MSG_LEN, type);
}