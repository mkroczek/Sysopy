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

int server_queue, private_queue, friend_queue;
int private_id;

void list(message* msg){
    printf("%s", msg->text);
}

void set_id(message* msg){
    private_id = msg->id;
    printf("Client received a respond message and has id = %d\n", private_id);
}

void connect(message* msg){
    friend_queue = atoi(msg->text);
    printf("Client received a respond message and connects to %d\n", friend_queue);
}

void disconnect(message* msg){
    friend_queue = -1;
    printf("%s", msg->text);
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
        message msg;
        msg.type = STOP;
        msg.id = private_id;
        send_msg(server_queue, &msg);
    }
    delete_queue(private_queue);
    exit(1);
}

void run_command(char* buf){
    char* pch;
    char* end;
    pch = strtok_r(buf, " \n\0", &end);
    if (strcmp(pch, "LIST") == 0){
        message msg;
        msg.type = LIST;
        msg.id = private_id;
        send_msg(server_queue, &msg);
    }
    else if (strcmp(pch, "CONNECT") == 0){
        pch = strtok_r(NULL, " \n\0", &end);
        message msg;
        msg.type = CONNECT;
        msg.id = private_id;
        strcpy(msg.text, pch);
        send_msg(server_queue, &msg);        
    }
    else if (strcmp(pch, "SEND") == 0){
        pch = strtok_r(NULL, "\0", &end);
        message msg;
        msg.type = RECEIVE;
        msg.id = private_id;
        strcpy(msg.text, pch);
        send_msg(friend_queue, &msg);
        
    }
    else if (strcmp(pch, "DISCONNECT") == 0){
        message msg;
        msg.type = DISCONNECT;
        msg.id = private_id;
        send_msg(server_queue, &msg);
        
    }
    else if (strcmp(pch, "STOP") == 0){
        stop();
        
    }
}

void receive_friend(message* msg){
    printf("From %d:\n%s", msg->id, msg->text);
}

void receive(){
    message msg;
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
        else if (receive_msg(private_queue, &msg) > 0){
            switch (msg.type)
            {
            case INIT:
                set_id(&msg);
                break;
            case LIST:
                list(&msg);
                break;
            case CONNECT:
                connect(&msg);
                break;
            case DISCONNECT:
                disconnect(&msg);
                break;
            case RECEIVE:
                receive_friend(&msg);
                break;
            case STOP:
                stop(&msg);
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
    key_t client_key = get_client_key();
    private_queue = create_queue(client_key);
    server_queue = get_queue(get_server_key());
    printf("Server queue in client is %d\n", server_queue);
    message msg;
    msg.id = private_queue;
    msg.type = INIT;
    printf("msg id = %d\n", msg.id);
    send_msg(server_queue, &msg);
    
}

int main (int argc, char** argv){

    atexit(stop);
    signal(SIGINT, end_handler);
    init();
    receive();
    return 0;
}