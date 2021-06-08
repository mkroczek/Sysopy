# include "utils.h"

void err_n_die(const char *fmt){

    int errno_save = errno;
    printf("%s\n", fmt);

    if (errno_save != 0){
        printf("errno = %d\n", errno_save);
    }

    exit(1);

}

message* parse_message(char* string){
    message* res = (message*) calloc(1, sizeof(message));
    res->cmd = (char*) calloc(1, MAX_CMD_LEN);
    res->msg = (char*) calloc(1, MAX_MESSAGE_LEN);
    res->name = (char*) calloc(1, MAX_NAME_LEN);
    char* pch;
    char* end;
    pch = strtok_r(string, ":", &end);
    strcpy(res->cmd, pch);
    pch = strtok_r(NULL, ":", &end);
    strcpy(res->name, pch);
    strcpy(res->msg, end);
    
    return res;
}

game_message* parse_game_message(char* string){
    game_message* res = (game_message*) calloc(1, sizeof(game_message));
    res->opponent = (char*) calloc(1, MAX_NAME_LEN);
    res->board = (char*) calloc(1, MAX_MESSAGE_LEN);
    char* pch;
    char* end;
    pch = strtok_r(string, ":", &end);
    strcpy(res->opponent, pch);
    pch = strtok_r(NULL, ":", &end);
    strcpy(res->board, pch);
    res->sign = end[0];
    
    return res;
}

message* create_message(char* cmd, char* name, char* msg){
    message* res = (message*) calloc(1, sizeof(message));
    res->cmd = (char*) calloc(1, MAX_CMD_LEN);
    res->msg = (char*) calloc(1, MAX_MESSAGE_LEN);
    res->name = (char*) calloc(1, MAX_NAME_LEN);
    strcpy(res->cmd, cmd);
    strcpy(res->name, name);
    strcpy(res->msg, msg);

    return res;
}

client* create_client(char* name){
    client* res = (client*) calloc(1, sizeof(client));
    res->name = (char*) calloc(1, MAX_NAME_LEN);
    res->game_id = -1;
    res->fd = -1;
    strcpy(res->name, name);

    return res;
}

game* create_game(int player1, int player2){
    game* new_game = (game*) calloc(1, sizeof(game));
    new_game->player1_id = player1;
    new_game->player1_id = player1;
    new_game->player1_sign = O;
    new_game->player2_sign = X;
    for (int i = 0; i < 9; i++){
        new_game->board[i] = FREE;
    }
    return new_game;
}

GAME_STATUS sign_to_status(SIGN sign){
    if (sign == X)
        return X_WON;
    else if (sign == O)
        return O_WON;
    else
        return RUNNING;
}

char sign_to_char(SIGN sign){
    switch (sign){
        case X:
            return 'X';
        case O:
            return 'O';
        
    }
}

GAME_STATUS get_game_status(game g){

    //check rows
    for (int i = 0; i < 3; i++){
        if (g.board[i*3] == g.board[i*3+1] && g.board[i*3] == g.board[i*3+2] && g.board[i*3] != FREE){
            return sign_to_status(g.board[i*3]);
        }
    }

    //check columns
    for (int i = 0; i < 3; i++){
        if (g.board[i] == g.board[i+3] && g.board[i] == g.board[i+6] && g.board[i] != FREE){
            return sign_to_status(g.board[i]);
        }
    }

    //check right diag
    if (g.board[0] == g.board[4] && g.board[0] == g.board[8] && g.board[0] != FREE){
        return sign_to_status(g.board[0]);
    }

    //check left diag
    if (g.board[2] == g.board[4] && g.board[2] == g.board[6] && g.board[2] != FREE){
        return sign_to_status(g.board[2]);
    }

    for (int i = 0; i < 9; i++){
        if (g.board[i] == FREE)
            return RUNNING;
    }

    return DRAW;

}

char* board_to_string(SIGN* board){
    char* result = (char*) calloc(10*20+1, sizeof(char));
    memset(result, ' ', 20*10);
    
    //create new line
    for(int i = 0; i < 10; i++){
        result[i*20 + 19] = '\n';
    }

    //create horizontal bars
    for(int i = 1; i < 6; i++){
        for (int j = 0; j < 4; j++){
            for (int z = 0; z < 3; z++){
                result[i+60*j+z*6] = '_';
            }
        }
    }

    //create vertical bars
    for(int i = 1; i < 10; i++){
        for (int j = 0; j < 4; j++){
            result[i*20+j*6] = '|';
        }
    }

    //fill cells
    for (int i = 0; i < 9; i++){
        char sign;
        switch (board[i]){
            case X:
                sign = 'X';
                break;
            case O:
                sign = 'O';
                break;
            default:
                sign = '0'+i;
        }
        result[43+(i%3)*6+(i/3)*60] = sign;
    }

    return result;

}

int can_make_move(int position, game* g){

}

void clear_msg(message* msg){
    free(msg->cmd);
    free(msg->msg);
    free(msg->name);
}

void clear_client(client* client){
    free(client->name);
}

void clear_game_message(game_message* gm){
    free(gm->opponent);
    free(gm->board);
}

char* game_status_to_string(GAME_STATUS status){
    switch (status){
        case O_WON:
            return "O won";
        case X_WON:
            return "X won";
        case DRAW:
            return "Draw";
        default:
            return "";
    }
}

char* message_to_string(message* msg){
    char* res = (char*) calloc(MAX_CMD_LEN+MAX_MESSAGE_LEN+MAX_NAME_LEN+3, sizeof(char));
    sprintf(res, "%s:%s:%s", msg->cmd, msg->name, msg->msg);
    return res;
}