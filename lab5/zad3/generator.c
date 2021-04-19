#include <stdio.h>

void fulfill(FILE* file, char c, int N){
    for (int i = 0; i < N; i++){
        fputc(c, file);
    }
}

int main(int argc, char* argv){
    FILE* files[10];
    char characters[10] = {'1', '2', '3', '4', '5', 'a', 'b', 'c', 'd', 'e'};
    files[0] = fopen("resources/1.txt", "w");
    files[1] = fopen("resources/2.txt", "w");
    files[2] = fopen("resources/3.txt", "w");
    files[3] = fopen("resources/4.txt", "w");
    files[4] = fopen("resources/5.txt", "w");
    files[5] = fopen("resources/a.txt", "w");
    files[6] = fopen("resources/b.txt", "w");
    files[7] = fopen("resources/c.txt", "w");
    files[8] = fopen("resources/d.txt", "w");
    files[9] = fopen("resources/e.txt", "w");

    for(int i = 0; i< 10; i++){
        fulfill(files[i], characters[i], 150);
        fclose(files[i]);
    }

    return 0;
}