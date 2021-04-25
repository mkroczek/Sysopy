#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "constants.h"
#include "keys.h"
#include "que_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int server_queue;
int clients[MAX_CLIENTS][2]; 
int n_clients = 0;

void add_client(message* msg){
    if (n_clients >= MAX_CLIENTS){
        perror("Max number of clients has already been reached!");
    }
    else{
        clients[n_clients][0] = msg->id;
        clients[n_clients][1] = AVAILABLE;
        message respond_msg;
        respond_msg.type = INIT;
        respond_msg.id = n_clients;
        send_msg(clients[n_clients][0], &respond_msg);
        printf("Server sent a confirmation message and set client id as %d\n", n_clients);
        n_clients ++;
    }
}

void list(message* msg){
    int id = msg->id;
    message* respond_msg = (message*) calloc(1, sizeof(message));
    respond_msg->type = LIST;
    respond_msg->id = server_queue;
    for(int i = 0; i < n_clients; i++){
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
    
    init();
    receive_server();
    delete_queue(server_queue);

    return 0;
}