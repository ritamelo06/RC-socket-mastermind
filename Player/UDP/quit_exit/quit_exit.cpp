#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <unistd.h>
#include "../../constants.hpp"
#include "../UDP.hpp"
#include "quit_exit.hpp"
using namespace std;
extern int tcp_fd;

bool verify_quit_exit(const vector<string>& tokens, Player* player) {
    
    string cmd = tokens[0];
    
    if (tokens.size() != 1 ) {
        cerr << "Invalid command.\n";
        return false;
    }
    else if (player->game_on == false) {
        if (cmd == "exit")
            exit(EXIT_SUCCESS);
        else if (cmd == "quit") {
            cerr << "Player does not have any ongoing game.\n";
            return false;
        }
    }
    return true;
}

int process_quit_exit(const string& input, Player* player, string& request) {
    vector<string> tokens = split_string(input, ' ');

    if (verify_quit_exit(tokens, player) == false) {
        return 1;   // 1??? pq nao quero repetir prints de erros mas idk
    }

    char buffer[MAX_BUFFER_SIZE];
    
    int result = snprintf(buffer, sizeof(buffer), "%s %d\n", QUIT_CMD, player->player_id);
    if (result < 0) {
        cerr << "Error processing quit command.\n";
        return -1;
    }

    request = string(buffer);
    return 0;
}

void print_quit_exit_response (const vector<string>& tokens, Player* player, bool is_exit) {
 
    if (tokens[1] == "OK") {
        
        printf("Game ended! Secret key: %s %s %s %s\n", tokens[2].c_str(),
        tokens[3].c_str(), tokens[4].c_str(), tokens[5].c_str());
        
        player->game_on = false;

        if (tcp_fd != -1) {
            close(tcp_fd);
            tcp_fd = -1;
        }
        if (is_exit) {
            exit(EXIT_SUCCESS);
        }
    } 
    else if (tokens[1] == "NOK") {
        if (is_exit)            
            exit(EXIT_FAILURE);
        else
            printf("[Error]: player does not have any ongoing game.\n");
    }
        
    else if (tokens[1] == "ERR") 
        printf("[Error]: could not exit game.\n");    
}