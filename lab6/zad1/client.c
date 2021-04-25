#include "keys.h"
#include "que_utils.h"
#include "constants.h"
#include <stdio.h>

int server_queue, private_queue;
int private_id;

void set_id(message* msg){
    private_id = msg->id;
    printf("Client received a respond message and has id = %d\n", private_id);
}

void receive(){
    message msg;
    while(1){
        receive_msg(private_queue, &msg);
        switch (msg.type)
        {
        case INIT:
            set_id(&msg);
            break;
        default:
            break;
        }
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
    
    init();
    receive();
    delete_queue(private_queue);
    return 0;
}