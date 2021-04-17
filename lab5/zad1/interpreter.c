#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node{
    char* argument;
    struct node* next;
};

struct command{
    char* prog;
    struct node* arguments;
};

struct command_node{
    struct command* command;
    struct command_node* next;
};

struct element{
    int id;
    struct command_node* commands;
};

void append(struct node* head, struct node* next){
    struct node* i = head;
    if (head == NULL){
        head = next;
        return;
    }
    while (i->next){
        i = i->next;
    }
    i->next = next;
}

void destroy(struct node* head){
    struct node* next;
    while (head){
        next = head->next;
        free(head->argument);
        free(head);
        head = next;
    }
}

void free_command(struct command* command){
    free(command->prog);
    destroy(command->arguments);
}

void append_command(struct command_node* head, struct command_node* next){
    struct command_node* i = head;
    if (head == NULL){
        head = next;
        return;
    }
    while (i->next){
        i = i->next;
    }
    i->next = next;
}

void destroy_commands(struct command_node* head){
    struct command_node* next;
    while (head){
        next = head->next;
        free_command(head->command);
        free(head);
        head = next;
    }
}

int getline_lib(char** line, size_t* length, FILE* file){
    //returns number of read characters
    char* buf = (char*) calloc(256, sizeof(char));
    char* ptr;
    char c;
    size_t current_buffer_size = 256;
    *length = 0;
    size_t i = 0;

    while(i < current_buffer_size){
        while(i < current_buffer_size && fread(&c, sizeof(char), 1, file) == 1){
            buf[i] = c;
            i += 1;
            if (c == '\n') break;
        }
        if (i == current_buffer_size){
            current_buffer_size *= 2;
            ptr = (char*) realloc(buf, current_buffer_size);
            buf = ptr;
        }
        else{
            break;
        }
    }

    *length = i;
    *line = buf;
    return i;
    
}

int get_element_id(char* element_name){
    return atoi(element_name+8);
    
}

struct command* parse_command(char* string){
    struct command* cmd = (struct command*) calloc(1, sizeof(struct command*));
    char* ptr;
    char* pch;
    pch = strtok(string, " ");
    cmd->prog = (char*) calloc(strlen(pch), sizeof(char));
    strcpy(cmd->prog, pch);
    pch = strtok(NULL, " ");
    while (pch){
        ptr = (char*) calloc(strlen(pch), sizeof(char));
        strcpy(ptr, pch);
        struct node* argument = (struct node*) calloc(1, sizeof(struct node*));
        argument->argument = ptr;
        argument->next = NULL;
        append(cmd->arguments, argument);
        pch = strtok(NULL, " ");
    }
    return cmd;
    // while (i < strlen(string)){
    //     int j = 0;
    //     while (string[i] != 0 && string[i] != 32 && string[i] != 124){
    //         while (j < buf_size && string[i] != 0 && string[i] != 32 && string[i] != 124){
    //             buf[j] = string[i];
    //             i++;
    //             j++;
    //         }
    //     }
    // }
}

struct element parse_element(char* string){
    struct element element;
    char* pch;
    element.commands = NULL;
    pch = strtok(string, "="); // gives element name
    element.id = get_element_id(pch);
    pch = strtok(NULL, "|");
    while (pch){
        struct command_node* command = (struct command_node*) calloc(1, sizeof(struct command_node*));
        command->command = parse_command(pch);
        command->next = NULL;
        append_command(element.commands, command);
        pch = strtok(NULL, "|");
    }
    return element;
    
}

int main(int argc, char** argv){
    char* commands_dir = argv[1];
    FILE* commands_file = fopen(commands_dir, "r");

    int comments_found = 0;
    struct element* ptr;

    int elements_arr_len = 256;
    struct element* elements = (struct element*) calloc(elements_arr_len, sizeof(struct element));

    char* line = NULL;
    size_t line_len = 0;

    int l = getline_lib(&line, &line_len, commands_file);
    int i = 0;

    while (l != 0){
        if (strchr(line, '#') != NULL){
            comments_found += 1;
        }
        else if (comments_found == 1){
            elements[i] = parse_element(line);
            i ++;
            if (i == elements_arr_len){
                elements_arr_len *= 2;
                ptr = (struct element*) realloc(elements, elements_arr_len);
                elements = ptr;
            }
        }
        else if (comments_found == 2){
            char* pcr = strtok(line, " |\n");
            while (pcr){
                int element_index = get_element_id(pcr);
                printf("Element index = %d\n", element_index);
                pcr = strtok(NULL, " |\n");
            }
        }
        l = getline_lib(&line, &line_len, commands_file);
        
    }

    fclose(commands_file);
    free(elements);

    return 0;
}