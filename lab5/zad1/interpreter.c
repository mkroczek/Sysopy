#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PROGRAMS 10

struct command{
    char* prog;
    char** arguments;
    int n_args;
};

struct command_node{
    struct command* command;
    struct command_node* next;
};

struct element{
    int id;
    int n_commands;
    struct command_node* commands;
};

void free_command(struct command* command){
    free(command->prog);
    for (int i = 0; i < command -> n_args; i++){
        free(command->arguments[i]);
    }

}

struct command_node* append_command(struct command_node* head, struct command_node* next){
    struct command_node* i = head;
    if (head == NULL){
        head = next;
        return head;
    }
    while (i->next){
        i = i->next;
    }
    i->next = next;
    return head;
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
    struct command* cmd = (struct command*) calloc(1, sizeof(struct command));
    int n_arguments = 40;
    int i = 0;
    char** arguments = (char**) calloc(n_arguments, sizeof(char*)); 
    char* ptr;
    char* pch;
    char** buf;
    char* end;
    pch = strtok_r(string, " \n", &end);
    cmd->prog = (char*) calloc(strlen(pch), sizeof(char));
    strcpy(cmd->prog, pch);
    pch = strtok_r(NULL, " \n", &end);
    while (pch){
        ptr = (char*) calloc(strlen(pch), sizeof(char));
        strcpy(ptr, pch);
        arguments[i] = ptr;
        i ++;
        if (i >= n_arguments){
            n_arguments *= 2;
            buf = (char**) realloc(arguments, n_arguments);
            arguments = buf;
        }
        pch = strtok_r(NULL, " \n", &end);
    }
    buf = (char**) realloc(arguments, i);
    arguments = buf;
    cmd->arguments = arguments;
    cmd->n_args = i;
    return cmd;
    
}

struct element parse_element(char* string){
    struct element element;
    char* buf; 
    char* pch;
    char* end;
    element.commands = NULL;
    element.n_commands = 0;
    pch = strtok_r(string, "=", &end); // gives element name
    element.id = get_element_id(pch);
    pch = strtok_r(NULL, "|", &end);
    while (pch){
        buf = (char*) calloc(strlen(pch+1), sizeof(char));
        strcpy(buf, pch);
        struct command_node* command = (struct command_node*) calloc(1, sizeof(struct command_node));
        command->command = parse_command(buf);
        command->next = NULL;
        if (command->command == NULL){
            printf("Couldn't create command.\n");
        }
        element.commands = append_command(element.commands, command);
        element.n_commands ++;
        if (element.commands == NULL){
            printf("Couldn't append command.\n");
        }
        pch = strtok_r(NULL, "|", &end);
        free(buf);
    }
    return element;
    
}

int** prepare_descriptors(int size){
    int** descriptors = (int **) calloc(size, sizeof(int*));
    for (int i = 0; i < size; i++){
        int* descriptor = (int*) calloc(2, sizeof(int));
        descriptors[i] = descriptor;
        descriptors[i][0] = -1;
        descriptors[i][1] = -1;
    }
    return descriptors;
}

void destroy_descriptors(int** descriptors, int size){
    for (int i = 0; i < size; i++){
        free(descriptors[i]);
    }
    free(descriptors);
    descriptors = NULL;
}

struct element create_superelement(char* line, struct element* elements){
    struct element superelement;
    superelement.n_commands = 0;
    superelement.commands = NULL;
    struct element current_el;
    char* pch;
    char* end;
    pch = strtok_r(line, " |\n", &end);
    while (pch){
        //one element
        int element_index = get_element_id(pch);
        current_el = elements[element_index-1];
        superelement.n_commands += current_el.n_commands;
        superelement.commands = append_command(superelement.commands, current_el.commands);
        pch = strtok_r(NULL, " |\n", &end);
    }
    return superelement;
}

void unzip_superelement(char* line, struct element* elements){
    struct element current_el;
    struct command_node* command_node;
    char* pch;
    char* end;
    pch = strtok_r(line, " |\n", &end);
    while (pch){
        //one element
        int element_index = get_element_id(pch);
        current_el = elements[element_index-1];
        command_node = current_el.commands;
        for(int i = 1; i < current_el.n_commands; i++){
            command_node = command_node->next;
        }
        command_node->next = NULL;
        pch = strtok_r(NULL, " |\n", &end);
    }
}

int main(int argc, char** argv){
    char* commands_dir = argv[1];
    FILE* commands_file = fopen(commands_dir, "r");

    int reading_elements = 1;
    struct element* ptr;

    int elements_arr_len = 256;
    struct element* elements = (struct element*) calloc(elements_arr_len, sizeof(struct element));

    char* line = NULL;
    size_t line_len = 0;

    int l = getline_lib(&line, &line_len, commands_file);
    int i = 0;

    while (l != 0){
        if (l == 1){
            reading_elements = 0;
        }
        else if (reading_elements == 1){
            elements[i] = parse_element(line);
            i ++;
            if (i == elements_arr_len){
                elements_arr_len *= 2;
                ptr = (struct element*) realloc(elements, elements_arr_len);
                elements = ptr;
            }
        }
        else if (reading_elements == 0){
            printf("Executing commands %s\n", line);
            struct element superelement = create_superelement(line, elements);
            int descriptors_size = superelement.n_commands - 1;
            int** descriptors = prepare_descriptors(descriptors_size);

            for (int j = 0; j < descriptors_size; j++){
                pipe(descriptors[j]);
            }

            int command_count = 0;

            struct command_node* command_node = superelement.commands;
            while(command_node){
                struct command* command = command_node->command;
                if (fork() == 0){
                    //prepare arguments
                    char** args = (char**) calloc(command->n_args + 2, sizeof(char*));
                    args[0] = command->prog;
                    for (int j = 0; j < command->n_args; j ++){
                        args[j+1] = command->arguments[j];
                    }
                    args[command->n_args+1] = NULL;

                    // //print
                    // for (int j = 0; j < command->n_args+2; j ++){
                    //     printf("Argument =%s\n", args[j]);
                    // }
                    // printf("\n");

                    if (command_count > 0){
                        dup2(descriptors[command_count-1][0], STDIN_FILENO);
                    }
                    if (command_count < descriptors_size){
                        dup2(descriptors[command_count][1], STDOUT_FILENO);
                    }
                    for (int p = 0; p < descriptors_size; p++){
                        if (p != command_count-1)
                            close(descriptors[p][0]);
                        if (p != command_count)
                            close(descriptors[p][1]);
                    }
                    execvp(command->prog, args);

                }
                command_count ++;
                command_node = command_node->next;

            }
            for (int j = 0; j < descriptors_size; j++){
                close(descriptors[j][0]);
                close(descriptors[j][1]);
            }
            for (int j = 0; j < superelement.n_commands; j++){
                wait(NULL);
            }
            unzip_superelement(line, elements);
            destroy_descriptors(descriptors, descriptors_size);

        }
        l = getline_lib(&line, &line_len, commands_file);
        
    }

    fclose(commands_file);
    for (i = i-1; i >= 0; i--){
        destroy_commands(elements[i].commands);
    }
    free(elements);

    return 0;
}