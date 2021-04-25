#include <sys/ipc.h>
#ifndef KEYS_H
#define KEYS_H
#define SERVER_KEY 's'

key_t get_server_key();
key_t get_client_key();


#endif