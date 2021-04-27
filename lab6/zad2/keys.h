#include <sys/ipc.h>
#ifndef KEYS_H
#define KEYS_H
#define PREFIX ("/queue-")

char* get_server_name();
char* get_client_name();
char* random_string(int len);
char* concat(char* str1, char* str2);

#endif