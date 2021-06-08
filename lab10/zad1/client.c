#include "utils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

char* name;
int client_socket;
GAME_STATUS game_status;
TURN turn;
pthread_t game_thread;

pthread_mutex_t variable_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t variable_cond = PTHREAD_COND_INITIALIZER;

void quit(){
    message* msg = create_message("quit", name, "");
    char* string = message_to_string(msg);
    send(client_socket, string, MAX_MSG_STR_LEN, 0);
    clear_msg(msg);
    free(msg);
    exit(0);
}

void init_server_connection_local(char* server_path){
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1)
        err_n_die("Couldn't create local socket for client");
    struct sockaddr_un local_sockaddr;
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, server_path);

    if(connect(client_socket, (struct sockaddr *)&local_sockaddr, sizeof(struct sockaddr_un)) != 0){
        err_n_die("Couldn't connect to the server\n");
    }

    message* msg = create_message("add", name, "");
    char* string = message_to_string(msg);
    send(client_socket, string, MAX_MSG_STR_LEN, 0);
    clear_msg(msg);
    free(msg);
}

void init_server_connection_network(int port, char* server_address){
    struct sockaddr_in server_sock;

    server_sock.sin_family = AF_INET;
    server_sock.sin_port = htons(port);
    server_sock.sin_addr.s_addr = inet_addr(server_address);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
        err_n_die("Couldn't create network socket for client");
    connect(client_socket, (struct sockaddr*) &server_sock, sizeof(server_sock));

    message* msg = create_message("add", name, "");
    char* string = message_to_string(msg);
    send(client_socket, string, MAX_MSG_STR_LEN, 0);
    clear_msg(msg);
    free(msg);

}

void get_message(){
    struct pollfd pfd;
    
    pfd.fd = client_socket;
    pfd.events = POLLIN;
    
    poll(&pfd, 1, -1);

}

void* play_game(){
    pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while(game_status == RUNNING){
        pthread_mutex_lock(&variable_mutex);
        while (turn == OPPONENT){
            pthread_cond_wait(&variable_cond, &variable_mutex);
        }
        pthread_mutex_unlock(&variable_mutex);
        char* input = (char*) calloc(2, sizeof(char));
        // char input;
        scanf("%s", input);
        //send information to server and change turn value
        pthread_mutex_lock(&variable_mutex);
        
        message* msg = create_message("move", name, input);
        char* string = message_to_string(msg);
        send(client_socket, string, MAX_MSG_STR_LEN, 0);
        clear_msg(msg);
        free(input);
        free(msg);
        turn = OPPONENT;

        pthread_mutex_unlock(&variable_mutex);

    }
    return NULL;
}

int main(int argc, char** argv){
    if (argc != 4 && argc != 5)
        err_n_die("Number of arguments must be 3 or 4");

    name = argv[1];
    char* service = argv[3];
    int server_port;
    game_status = END;
    turn = OPPONENT;

    signal(SIGINT, quit);

    if (strcmp(argv[2], "local") == 0)
        init_server_connection_local(service);
    else if (strcmp(argv[2], "network") == 0){
        server_port = atoi(argv[4]);
        init_server_connection_network(server_port, service);
    }
    else
        err_n_die("Type of connection must be local or network");

    while(1){
        get_message();
        message* msg;
        char buf[MAX_MSG_STR_LEN];
        recv(client_socket, buf, MAX_MSG_STR_LEN, 0);
        msg = parse_message(buf);
        if (strcmp(msg->cmd, "add") == 0){
            printf("%s\n",msg->msg);
        }
        else if (strcmp(msg->cmd, "start") == 0){
            printf("%s", msg->msg);
            pthread_mutex_lock(&variable_mutex);
            game_status = RUNNING;
            pthread_create(&game_thread, NULL, &play_game, NULL);
            pthread_mutex_unlock(&variable_mutex);
            
        }
        else if (strcmp(msg->cmd, "end") == 0){
            printf("%s\n", msg->msg);
            pthread_cancel(game_thread);
            game_status = END;
            turn = OPPONENT;
            quit();
            
        }
        else if (strcmp(msg->cmd, "move") == 0){
            printf("%s", msg->msg);
            pthread_mutex_lock(&variable_mutex);
            turn = MY;
            pthread_cond_signal(&variable_cond);
            pthread_mutex_unlock(&variable_mutex);
            
        }
        else if (strcmp(msg->cmd, "opponent_move") == 0){
            printf("%s", msg->msg);
            
        }
        clear_msg(msg);
        free(msg);
    }

    return 0;

}