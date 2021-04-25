#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "constants.h"
#include "keys.h"
#include "que_utils.h"
#include <stdio.h>
#include <stdlib.h>

int server_queue;
int clients[MAX_CLIENTS]; 
int n_clients = 0;

void add_client(message* msg){
    printf("I am in add_client\n");
    if (n_clients >= MAX_CLIENTS){
        perror("Max number of clients has already been reached!");
    }
    else{
        clients[n_clients] = msg->id;
        message respond_msg;
        respond_msg.type = INIT;
        respond_msg.id = n_clients;
        send_msg(clients[n_clients], &respond_msg);
        printf("Server sent a confirmation message and set client id as %d\n", n_clients);
        n_clients ++;
    }
}

void receive_server(){
    message msg;
    while(1){
        receive_msg(server_queue, &msg);
        switch (msg.type)
        {
        case INIT:
            add_client(&msg);
            break;
        
        default:
            break;
        }
    }

}


void init(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i] = -1;
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