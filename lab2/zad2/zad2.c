#include <stdio.h>
#include <stdlib.h>

int contains_chr(char character, char* buffer, int length){
    for (int i = 0; i < length && buffer[i] != 10; i+=1){
        if(buffer[i] == character) return 1;
    }
    return 0;
}

void print_containing(char character, char* filename){
    FILE* file = fopen(filename, "r");
    char* buffer = (char*) malloc(256);
    if(file == NULL){
        perror("Couldn't open file");
        return;
    }
    while(fgets(buffer, 256, file)){
        if (contains_chr(character, buffer, 256) == 1) printf(buffer);
    }

}

int main(int argc, char** argv){
    char character;
    char* filename;

    character = argv[1];
    filename = argv[2];

    return 0;
}