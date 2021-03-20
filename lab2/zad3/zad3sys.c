#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
    FILE* data = fopen("dane.txt", "r");
    FILE* a = fopen("a.txt","w");
    FILE* b = fopen("b.txt", "w");

    return 0;
}