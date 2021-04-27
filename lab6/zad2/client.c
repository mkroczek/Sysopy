#include "keys.h"
#include "que_utils.h"
#include "constants.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <signal.h>

char private_name[20];
mqd_t server_queue, private_queue, friend_queue;
int private_id;

void list(char* msg){
    printf("%s", msg);
}

void set_id(char* msg){
    private_id = atoi(msg);
    printf("Client received a respond message and has id = %d\n", private_id);
}

void connect(char* msg){
    friend_queue = get_queue(msg);
    printf("Client received a respond message and connects to %d\n", friend_queue);
}

void disconnect(char* msg){
    close_queue(friend_queue);
    friend_queue = -1;
    printf("%s", msg);
}

int get_input(char* buf, int buf_len){
    int n_char = 0;
    n_char = read(0, buf, buf_len-1);
    if (n_char == 0){
        return 0;
    }
    buf[n_char] = '\0';
    return n_char;
}

void stop(){
    if(server_queue >= 0){
        char* msg = (char*) calloc(10, sizeof(char));
        sprintf(msg, "%d", private_id);
        send_msg(server_queue, msg, STOP);
    }
    close_queue(server_queue);
    close_queue(private_queue);
    delete_queue(private_name);
    exit(1);
}

void run_command(char* buf){
    char* pch;
    char* end;
    pch = strtok_r(buf, " \n\0", &end);
    if (strcmp(pch, "LIST") == 0){
        char* msg = (char*) calloc(10, sizeof(char));
        sprintf(msg, "%d", private_id);
        send_msg(server_queue, msg, LIST);
    }
    else if (strcmp(pch, "CONNECT") == 0){
        pch = strtok_r(NULL, " \n\0", &end);
        char* msg = (char*) calloc(20, sizeof(char));
        sprintf(msg, "%d", private_id);
        sprintf(msg, "%d %s", private_id, pch);
        send_msg(server_queue, msg, CONNECT);        
    }
    else if (strcmp(pch, "SEND") == 0){
        pch = strtok_r(NULL, "\0", &end);
        char* msg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
        sprintf(msg, "%d %s", private_id, pch);
        send_msg(friend_queue, msg, RECEIVE);
        
    }
    else if (strcmp(pch, "DISCONNECT") == 0){
        char* msg = (char*) calloc(10, sizeof(char));
        sprintf(msg, "%d", private_id);
        send_msg(server_queue, msg, DISCONNECT);
        
    }
    else if (strcmp(pch, "STOP") == 0){
        stop();
        
    }
}

void receive_friend(char* msg){
    char* pch;
    char* end;
    int friend_id;
    pch = strtok_r(msg, " \n\0", &end);
    friend_id = atoi(pch);
    pch = strtok_r(NULL, "\0", &end);
    printf("From %d:\n%s", friend_id, pch);
}

void receive(){
    char* msg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    int type;
    fd_set readfds, savedfds;
    int sret;
    char buf[25];
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    savedfds = readfds;
    while(1){
        sret = select(8, &readfds, NULL, NULL, &timeout);
        if (sret > 0){
            get_input(buf ,25);
            run_command(buf);
        }
        else if (receive_msg(private_queue, msg, &type) > 0){
            switch (type)
            {
            case INIT:
                set_id(msg);
                break;
            case LIST:
                list(msg);
                break;
            case CONNECT:
                connect(msg);
                break;
            case DISCONNECT:
                disconnect(msg);
                break;
            case RECEIVE:
                receive_friend(msg);
                break;
            case STOP:
                stop();
                break;
            default:
                break;
            }
        }
        readfds = savedfds;
    }

}

void end_handler(int signum){
    if (signum == SIGINT){
        stop();
    }
}

void init(){
    strcpy(private_name, get_client_name());
    private_queue = create_queue(private_name);
    server_queue = get_queue(get_server_name());
    printf("Server queue in client is %d\n", server_queue);
    send_msg(server_queue, private_name, INIT);
    
}

int main (int argc, char** argv){

    atexit(stop);
    signal(SIGINT, end_handler);
    init();
    receive();
    return 0;
}