#include "utils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BACKLOG 10

client* clients[MAX_CLIENTS];
game* games[MAX_CLIENTS/2];

int TCP_port;
int local_socket;
int network_socket;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int get_enemy_id(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL && clients[i]->game_id == -1){
            return i;
        }
    }

    return -1;
}

int get_clients_count(){
    int clients_count = 0;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL){
            clients_count ++;
        }
    }

    return clients_count;
}

int get_client_index(char* name){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0){
            return i;
        }
    }

    return -1;
}

int get_free_index(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] == NULL)
            return i;
    }

    return -1;
}

int get_free_index_game(){
    for (int i = 0; i < MAX_CLIENTS/2; i++){
        if (games[i] == NULL)
            return i;
    }

    return -1;
}

void remove_game(int g_id){
    clients[games[g_id]->player1_id]->game_id = -1;
    clients[games[g_id]->player2_id]->game_id = -1;
    games[g_id] = NULL;
}

void remove_client(char* name){
    int index = get_client_index(name);
    if (index != -1){
        int g_id = clients[index]->game_id;
        if (g_id != -1){
            int opponent_id;
            games[g_id]->player1_id == index ? (opponent_id = games[g_id]->player2_id) : (opponent_id = games[g_id]->player1_id);
            char* game_over_info = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
            sprintf(game_over_info, "Game over!\n %s left the server.\n", clients[index]->name);
            message* respond = create_message("end", "server", game_over_info);
            if (send(clients[opponent_id]->fd, message_to_string(respond), MAX_MSG_STR_LEN, 0) < 0)
                printf("[SERVER] Nie moge wyslac wiadomosci\n");
            free(game_over_info);
            clear_msg(respond);
            free(respond);
            remove_game(g_id);

        }
        clear_client(clients[index]);
        clients[index] = NULL;
    }
}

int add_client(char* name, int client_fd){
    int index = get_free_index();

    if (index != -1){
        clients[index] = create_client(name);
        clients[index]->fd = client_fd;
    }

    return index;

}

int get_message(){
    pthread_mutex_lock(&mutex);
    int clients_count = get_clients_count();
    struct pollfd* fds = (struct pollfd*) calloc(2+clients_count, sizeof(struct pollfd));
    int socket_id = -1;

    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;
    int i = 2;
    for (int j = 0; j < MAX_CLIENTS && i < clients_count + 2; j++){
        if (clients[j] != NULL){
            fds[i].fd = clients[j]->fd;
            fds[i].events = POLLIN;
            i ++;
        }
    }
    pthread_mutex_unlock(&mutex);
    poll(fds, clients_count + 2, -1);
    for (i = 0; i < clients_count + 2; i++){
        if (fds[i].revents & POLLIN)
        {
            socket_id = fds[i].fd;
            break;
        }
    }
    if (socket_id == local_socket || socket_id == network_socket){
        //client wants to connect
        socket_id = accept(socket_id, NULL, NULL);
    }
    free(fds);
    return socket_id;

}

void send_move_info(int move_player_id, int wait_player_id, char move_player_sign, char wait_player_sign, char* board){
    message* respond_move_player;
    message* respond_wait_player;
    char* move_player_info = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
    char* wait_player_info = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
    sprintf(wait_player_info, "It's Your opponent move!\nopponent: %s\nboard:\n%s\nsign: %c\n", clients[move_player_id]->name, board, wait_player_sign);
    respond_wait_player = create_message("opponent_move", "server", wait_player_info);
    sprintf(move_player_info, "It's Your move!\nopponent: %s\nboard:\n%s\nsign: %c\n", clients[wait_player_id]->name, board, move_player_sign);
    respond_move_player = create_message("move", "server", move_player_info);
    send(clients[wait_player_id]->fd, message_to_string(respond_wait_player), MAX_MSG_STR_LEN, 0);
    send(clients[move_player_id]->fd, message_to_string(respond_move_player), MAX_MSG_STR_LEN, 0);
    clear_msg(respond_wait_player);
    clear_msg(respond_move_player);
    free(respond_wait_player);
    free(respond_move_player);
    free(move_player_info);
    free(wait_player_info);

}

void send_end_info(int player1, int player2, int game_id, GAME_STATUS end_status){
    message* respond_player = create_message("end", "server", game_status_to_string(end_status));
    remove_game(game_id);
    send(clients[player1]->fd, message_to_string(respond_player), MAX_MSG_STR_LEN, 0);
    send(clients[player2]->fd, message_to_string(respond_player), MAX_MSG_STR_LEN, 0);
    clear_msg(respond_player);
    free(respond_player);

}

int init_local_socket(char *socket_path){
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (local_socket == -1)
        err_n_die("Couldn't create local socket for server");
    struct sockaddr_un local_sockaddr;
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, socket_path);
    unlink(socket_path);
    if(bind(local_socket, (struct sockaddr*)&local_sockaddr, sizeof(struct sockaddr_un)) == -1)
        err_n_die("Couldn't bind local socket");
    listen(local_socket, BACKLOG);
    return local_socket;

}

int init_network_socket(int port){
    struct hostent* server_entry = gethostbyname("localhost");
    struct in_addr server_address = *(struct in_addr*) server_entry->h_addr;

    struct sockaddr_in network_sockaddr;
    network_sockaddr.sin_family = AF_INET;
    network_sockaddr.sin_port = htons(port);
    network_sockaddr.sin_addr.s_addr = server_address.s_addr;

    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1)
        err_n_die("Couldn't create network socket for server");
    if(bind(network_socket, (struct sockaddr*) &network_sockaddr, sizeof(network_sockaddr)) == -1)
        err_n_die("Couldn't bind network socket");
    if(listen(network_socket, BACKLOG) == -1)
        err_n_die("Couldn't listen");

    printf("Server network socket listening on %s:%d\n", inet_ntoa(server_address), port);
    return network_socket;

}

int main(int argc , char** argv){

    if(argc != 3){
        err_n_die("Number of arguments must be 2");
    }

    TCP_port = atoi(argv[1]);
    char* socket_path = argv[2];

    local_socket = init_local_socket(socket_path);
    network_socket = init_network_socket(TCP_port);

    while(1){
        int client_fd = get_message();
        char buf[MAX_MSG_STR_LEN];
        read(client_fd, buf, MAX_MSG_STR_LEN);
        message* msg = parse_message(buf);
        pthread_mutex_lock(&mutex);
        if (strcmp(msg->cmd, "add") == 0){
            if (get_client_index(msg->name) != -1){
                message* respond = create_message("add", "server", "name is already taken");
                send(client_fd, message_to_string(respond), sizeof(message), 0);
                close(client_fd);
                clear_msg(respond);
                free(respond);
            }
            else{
                int enemy_id = get_enemy_id();
                int given_index = add_client(msg->name, client_fd);
                if (enemy_id == -1){
                    message* respond = create_message("add", "server", "waiting for enemy");
                    send(client_fd, message_to_string(respond), MAX_MSG_STR_LEN, 0);
                    clear_msg(respond);
                    free(respond);
                }
                else{
                    int free_game_id = get_free_index_game();
                    games[free_game_id] = create_game(given_index, enemy_id);
                    game* g = games[free_game_id];
                    char* game_init_info = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
                    sprintf(game_init_info, "Game is starting!\nopponent: %s\nboard:\n%s\nsign: %c\n", clients[enemy_id]->name, board_to_string(g->board), sign_to_char(g->player1_sign));
                    message* respond = create_message("start", "server", game_init_info);
                    send(client_fd, message_to_string(respond), MAX_MSG_STR_LEN, 0);
                    memset(game_init_info, 0, strlen(game_init_info));
                    sprintf(game_init_info, "Game is starting!\nopponent: %s\nboard:\n%s\nsign: %c\n", clients[given_index]->name, board_to_string(g->board), sign_to_char(g->player2_sign));
                    clear_msg(respond);
                    respond = create_message("start", "server", game_init_info);
                    send(clients[enemy_id]->fd, message_to_string(respond), MAX_MSG_STR_LEN, 0);
                    clients[enemy_id]->game_id = free_game_id;
                    clients[given_index]->game_id = free_game_id;
                    clear_msg(respond);
                    free(game_init_info);
                    free(respond);

                    send_move_info(g->player1_id, g->player2_id, sign_to_char(g->player1_sign), sign_to_char(g->player2_sign), board_to_string(g->board));
                }
            }
        }
        else if(strcmp(msg->cmd, "quit") == 0){
            remove_client(msg->name);
        }
        else if(strcmp(msg->cmd, "move") == 0){
            int moved_index = get_client_index(msg->name);
            int g_id = clients[moved_index]->game_id;
            int opponent_index;
            int player_id;
            games[g_id]->player1_id == moved_index ? (player_id = 1) : (player_id = 2);
            player_id == 1 ? (opponent_index = games[g_id]->player2_id) : (opponent_index = games[g_id]->player1_id);
            int place = atoi(msg->msg);
            SIGN sign;
            SIGN opponent_sign;
            if (games[g_id]->board[place] == FREE){
                if (player_id == 1){
                    sign = games[g_id]->player1_sign;
                    opponent_sign = games[g_id]->player2_sign;
                }
                else{
                    sign = games[g_id]->player2_sign;
                    opponent_sign = games[g_id]->player1_sign;
                }
                games[g_id]->board[place] = sign;
            }

            //check for end of the game
            GAME_STATUS gs = get_game_status(*games[g_id]);
    
            switch (gs){
                case RUNNING:
                    send_move_info(opponent_index, moved_index, sign_to_char(opponent_sign), sign_to_char(sign), board_to_string(games[g_id]->board));
                    break;
                default:
                    send_end_info(moved_index, opponent_index, g_id, gs);
            }
            
        }
        clear_msg(msg);
        free(msg);
        pthread_mutex_unlock(&mutex);
    }
    return 0;

}