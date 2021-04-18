#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if (argc == 2){
        if (strcmp(argv[1], "nadawca") == 0){
            FILE* mail_input = popen("echo q | mail | sort -k 3", "w");
            pclose(mail_input);
        }
        else if(strcmp(argv[1], "data") == 0){
            FILE* mail_input = popen("echo q | mail | sort -M", "w");
            pclose(mail_input);
        }
    }
    else if (argc == 4){
        char* mail_adress = argv[1];
        char* title = argv[2];
        char* contents = argv[3];
        char* command = (char*) calloc(9+strlen(title)+strlen(mail_adress), sizeof(char));
        strcat(command, "mail -s ");
        strcat(command, title);
        strcat(command, " ");
        strcat(command, mail_adress);

        FILE* mail_input = popen(command, "w");
        fputs(contents, mail_input);
        pclose(mail_input);
        free(command);

    }
    return 0;

}