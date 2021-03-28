#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int contains_str(FILE* file, const char* string){
    char* line = NULL;
    size_t length = 0;
    
    while(getline(&line, &length, file) != -1){
        if (strstr(line, string) != NULL) return 1;
    }
    return 0;
}

int search_dir(const char* path, const char* string, int depth){
    if (depth <= 0) return 0;
    int no_found = 0;
    DIR* directory = opendir(path);
    FILE* file;
    if (directory == NULL){
        return no_found;
    }
    struct dirent* position = readdir(directory);
    struct stat statistic;
    while(position != NULL){
        char* obj = calloc(strlen(path)+strlen(position->d_name)+1, sizeof(char));
        strcpy(obj, path);
        strcat(obj, "/");
        strcat(obj, position->d_name);
        stat(obj, &statistic);
        if (S_ISDIR(statistic.st_mode) && (strcmp(".", position->d_name) != 0) && (strcmp("..", position->d_name) != 0)){
            if(fork() == 0){
                no_found += search_dir(obj, string, depth-1);
                _exit(0);
            }
        }
        if (S_ISREG(statistic.st_mode)){
            if (strstr(position->d_name, ".txt") != NULL){
                file = fopen(obj, "r");
                if(contains_str(file, string) != 0){
                    printf("%s PID = %d\n", obj, getpid());
                    no_found += 1;
                }
                fclose(file);
            }
        }
        free(obj);
        position = readdir(directory);
    }
    closedir(directory);
    return no_found;
}

int search(const char* path, const char* string, int depth){
    if (chdir(path) != 0){
        return -1;
    }
    else{
        if(fork() == 0){
            return search_dir(".", string, depth);
            _exit(0);
        }
    }
}

int main(int argc, char** argv){
    if(argc != 4) return -1;
    const char* path = argv[1];
    char* string = argv[2];
    int depth = atoi(argv[3]);
    search(path, string, depth);
    wait(NULL);

    return 0;
}