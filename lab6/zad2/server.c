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

typedef struct client{
    char client_name[20];
    mqd_t client_mqdes;
    int availability;
} client;

mqd_t server_queue;
client clients[MAX_CLIENTS]; 
int n_clients = 0;

int find_free_id(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].client_mqdes == AVAILABLE)
            return i;
    }
    return -1;
}

void clear_name(char* array){
    memset(array, 0, 20);

}

void add_client(char* msg){
    if (n_clients >= MAX_CLIENTS){
        perror("Max number of clients has already been reached!");
    }
    else{
        int free_id = find_free_id();
        printf("Server received client name = %s\n", msg);
        strcpy(clients[free_id].client_name, msg);
        clients[free_id].availability = AVAILABLE;
        clients[free_id].client_mqdes = get_queue(clients[free_id].client_name);
        char* respond_msg = (char*) calloc(10, sizeof(char));
        sprintf(respond_msg, "%d", free_id);
        send_msg(clients[free_id].client_mqdes, respond_msg, INIT);
        printf("Server sent a confirmation message and set client id as %d\n", n_clients);
        n_clients ++;
    }
}

void list(char* msg){
    int id = atoi(msg);
    char* respond_msg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].client_mqdes != AVAILABLE && i != id){
            char* info = (char*) calloc(100, sizeof(int));
            if (clients[i].availability == AVAILABLE){
                sprintf(info, "User %d is available\n",i);
            }
            else{
                sprintf(info, "User %d is not available\n",i);
            }
            strcat(respond_msg, info);
            free(info);
        }
    }
    send_msg(clients[id].client_mqdes, respond_msg, LIST);
    printf("Server sent list to client id %d\n", id);
}

void connect(char* msg){
    printf("Server is in command function\n");
    char* pch;
    char* end;
    pch = strtok_r(msg, " \n\0", &end);
    int sender_id = atoi(pch);
    pch = strtok_r(NULL, " \n\0", &end);
    int friend_id = atoi(pch);
    char* respond_msg_sender = (char*) calloc(20, sizeof(char));
    char* respond_msg_friend = (char*) calloc(20, sizeof(char));
    if (clients[friend_id].client_mqdes != AVAILABLE && clients[friend_id].availability == AVAILABLE){
        printf("Client is available\n");
        strcpy(respond_msg_sender, clients[friend_id].client_name);
        strcpy(respond_msg_friend, clients[sender_id].client_name);
        // respond_msg_sender[20] = '\0';
        // respond_msg_friend[20] = '\0';
        clients[friend_id].availability = sender_id;
        clients[sender_id].availability = friend_id;
        send_msg(clients[sender_id].client_mqdes, respond_msg_sender, CONNECT);
        send_msg(clients[friend_id].client_mqdes, respond_msg_friend, CONNECT);

    }
    else{
        printf("Client is not available\n");
        strcat(respond_msg_sender, "-1");
        send_msg(clients[sender_id].client_mqdes, respond_msg_sender, CONNECT);
    }
    printf("Server sent connetion information to clients\n");
}

void disconnect(char* msg){
    int sender_id = atoi(msg);
    int friend_id = clients[sender_id].availability;
    if (friend_id != AVAILABLE){
        clients[sender_id].availability = AVAILABLE;
        clients[friend_id].availability = AVAILABLE;
        char* respond_msg = (char*) calloc(22, sizeof(char));
        strcpy(respond_msg, "Chat has been closed\n");
        send_msg(clients[sender_id].client_mqdes, respond_msg, DISCONNECT);
        send_msg(clients[friend_id].client_mqdes, respond_msg, DISCONNECT);
    }
}

void stop_client(char* msg){
    int id = atoi(msg);
    if(clients[id].availability != AVAILABLE)
        disconnect(msg);
    close_queue(clients[id].client_mqdes);
    clients[id].client_mqdes = AVAILABLE;
    clear_name(clients[id].client_name);
    n_clients --;
    printf("Client %d stopped\n", id);

}

int all_ended(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].client_mqdes != AVAILABLE){
            // printf("Client %d didn't end\n", i);
            return 0;
        }
    }
    return 1;
}

void stop(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].client_mqdes != AVAILABLE){
            char* msg = (char*) calloc(1, sizeof(char));
            strcpy(msg, "");
            send_msg(clients[i].client_mqdes, msg, STOP);
        }
    }
    char* msg = (char*) calloc(MAX_MSG_LEN, sizeof(char));
    while (all_ended() == 0){
        int type;
        if(receive_msg(server_queue, msg, &type) > 0 && type == STOP){
            stop_client(msg);
        }
    }
    delete_queue(get_server_name());
    exit(1);
}

void end_handler(int signum){
    if (signum == SIGINT){
        stop();
    }
}

void receive_server(){
    char msg[MAX_MSG_LEN];
    int type;
    while(1){
        if(receive_msg(server_queue, msg, &type) > 0){
            switch (type)
            {
            case INIT:
                add_client(msg);
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
            case STOP:
                stop_client(msg);
                break;
            default:
                break;
            }
        }
    }

}

void init(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].client_mqdes = AVAILABLE;
        clients[i].availability = AVAILABLE;
        clear_name(clients[i].client_name);
    }
    char* server_name = get_server_name();
    printf("Server name = %s\n", server_name);
    server_queue = create_queue(server_name);

}

int main (int argc, char** argv){
    
    atexit(stop);
    signal(SIGINT, end_handler);
    init();
    receive_server();

    return 0;
}