#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "constants.h"
#include "keys.h"
#include "que_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int server_queue;
int clients[MAX_CLIENTS][2]; 
int n_clients = 0;

int find_free_id(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i][0] == -1)
            return i;
    }
    return -1;
}

void add_client(message* msg){
    if (n_clients >= MAX_CLIENTS){
        perror("Max number of clients has already been reached!");
    }
    else{
        int free_id = find_free_id();
        clients[free_id][0] = msg->id;
        clients[free_id][1] = AVAILABLE;
        message respond_msg;
        respond_msg.type = INIT;
        respond_msg.id = free_id;
        send_msg(clients[free_id][0], &respond_msg);
        printf("Server sent a confirmation message and set client id as %d\n", n_clients);
        n_clients ++;
    }
}

void list(message* msg){
    int id = msg->id;
    message* respond_msg = (message*) calloc(1, sizeof(message));
    respond_msg->type = LIST;
    respond_msg->id = server_queue;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i][0] != -1){
            char* info = (char*) calloc(100, sizeof(int));
            if (clients[i][1] == AVAILABLE){
                sprintf(info, "User %d is available\n",i);
            }
            else{
                sprintf(info, "User %d is not available\n",i);
            }
            strcat(respond_msg->text, info);
            free(info);
        }
    }
    send_msg(clients[id][0], respond_msg);
    printf("Server sent list to client id %d\n", id);
}

void connect(message* msg){
    printf("Server is in comman function\n");
    int sender_id = msg->id;
    int friend_id = atoi(msg->text);
    message* respond_msg_sender = (message*) calloc(1, sizeof(message));
    message* respond_msg_friend = (message*) calloc(1, sizeof(message));
    respond_msg_sender->type = CONNECT;
    respond_msg_sender->id = server_queue;
    respond_msg_friend->type = CONNECT;
    respond_msg_friend->id = server_queue;
    char* sender_qid = (char*) calloc(10, sizeof(int));
    char* friend_qid = (char*) calloc(10, sizeof(int));
    if (clients[friend_id][0] != -1 && clients[friend_id][1] == AVAILABLE){
        printf("Client is in available\n");
        sprintf(friend_qid, "%d",clients[friend_id][0]);
        sprintf(sender_qid, "%d",clients[sender_id][0]);
        clients[friend_id][1] = sender_id;
        clients[sender_id][1] = friend_id;
        strcat(respond_msg_sender->text, friend_qid);
        strcat(respond_msg_friend->text, sender_qid);
        send_msg(clients[sender_id][0], respond_msg_sender);
        send_msg(clients[friend_id][0], respond_msg_friend);

    }
    else{
        printf("Client is not available\n");
        strcat(respond_msg_sender->text, "-1");
        send_msg(clients[sender_id][0], respond_msg_sender);
    }
    free(sender_qid);
    free(friend_qid);
    printf("Server sent connetion information to clients\n");
}

void disconnect(message* msg){
    int sender_id = msg->id;
    int friend_id = clients[sender_id][1];
    if (friend_id != AVAILABLE){
        clients[sender_id][1] = AVAILABLE;
        clients[friend_id][1] = AVAILABLE;
        message respond_msg;
        respond_msg.id = server_queue;
        strcpy(respond_msg.text, "Chat has been closed\n");
        respond_msg.type = DISCONNECT;
        send_msg(clients[sender_id][0], &respond_msg);
        send_msg(clients[friend_id][0], &respond_msg);
    }
}

void stop_client(message* msg){
    int id = msg->id;
    if(clients[id][1] != AVAILABLE)
        disconnect(msg);
    clients[id][0] = -1;
    n_clients --;
    printf("Client %d stopped\n", id);

}

int all_ended(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i][0] != -1){
            // printf("Client %d didn't end\n", i);
            return 0;
        }
    }
    return 1;
}

void stop(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i][0] != -1){
            message msg;
            msg.id = server_queue;
            msg.type = STOP;
            send_msg(clients[i][0], &msg);
        }
    }
    while (all_ended() == 0){
        message msg;
        if(receive_msg(server_queue, &msg) > 0 && msg.type == STOP){
            stop_client(&msg);
        }
    }
    delete_queue(server_queue);
    exit(1);
}

void end_handler(int signum){
    if (signum == SIGINT){
        stop();
    }
}

void receive_server(){
    message msg;
    while(1){
        if(receive_msg(server_queue, &msg) > 0){
            switch (msg.type)
            {
            case INIT:
                add_client(&msg);
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
            case STOP:
                stop_client(&msg);
                break;
            default:
                break;
            }
        }
    }

}

void init(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i][0] = -1;
        clients[i][1] = AVAILABLE;
    }
    key_t server_key = get_server_key();
    server_queue = create_queue(server_key);
    printf("Server queue in server is %d\n",server_queue);

}

int main (int argc, char** argv){
    
    atexit(stop);
    signal(SIGINT, end_handler);
    init();
    receive_server();

    return 0;
}