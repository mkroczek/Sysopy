#include "keys.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

key_t get_server_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), SERVER_KEY)) == -1){
        perror("Couldn't generate server key\n");
    }
    return key;
}

key_t get_client_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), getpid())) == -1){
        perror("Couldn't generate client key\n");
    }
    return key;
}