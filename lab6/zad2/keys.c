#include "keys.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* get_server_name(){
    return concat(PREFIX, "server");
}

char* get_client_name(){
    return concat(PREFIX, random_string(12));
}

char* concat(char* str1, char* str2){
    char* result = (char*) calloc(strlen(str1)+strlen(str2)+1, sizeof(char));
    strcat(result, str1);
    strcat(result, str2);
    return result;
}

char* random_string(int len){
    int seed;
    time_t tt;
    seed = time(&tt);
    srand(seed);
    char* string = (char*) calloc(len+1, sizeof(char));
    for (int i = 0; i < len; i++){
        string[i] = 'a' + (rand() % 26);
    }
    string[len] = '\0';
    return string;
}