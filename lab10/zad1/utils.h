# ifndef UTILS_H
# define UTILS_H

# include <stdio.h>
# include <errno.h>
# include <stdlib.h>
#include <string.h>

#define MAX_CLIENTS 20
#define MAX_MESSAGE_LEN 400
#define MAX_NAME_LEN 30
#define MAX_CMD_LEN 15
#define MAX_MSG_STR_LEN MAX_MESSAGE_LEN+MAX_NAME_LEN+MAX_CMD_LEN+3

typedef struct client{
    int fd;
    char* name;
    int game_id;
}client;

typedef enum SIGN{
    O = 0,
    X = 1,
    FREE = 2
}SIGN;

typedef enum GAME_STATUS{
    RUNNING = 0,
    O_WON = 1,
    X_WON = 2,
    DRAW = 3,
    END = 4
}GAME_STATUS;

typedef enum TURN{
    MY,
    OPPONENT
}TURN;

typedef struct game{
    int player1_id;
    int player2_id;
    SIGN player1_sign;
    SIGN player2_sign;
    SIGN board[9];
}game;

typedef struct message{
    char* cmd;
    char* name;
    char* msg;
}message;

typedef struct game_message{
    char* opponent;
    char* board;
    char sign;
}game_message;

void err_n_die(const char *fmt);

message* parse_message(char* string);
game_message* parse_game_message(char* string);
message* create_message(char* cmd, char* name, char* msg);
client* create_client(char* name);
char* message_to_string(message* msg);

game* create_game(int player1, int player2);
GAME_STATUS sign_to_status(SIGN sign);
char sign_to_char(SIGN sign);
GAME_STATUS get_game_status(game g);
char* board_to_string(SIGN* board);

char* game_status_to_string(GAME_STATUS status);

void clear_msg(message* msg);
void clear_client(client* client);
void clear_game_message(game_message* gm);

# endif