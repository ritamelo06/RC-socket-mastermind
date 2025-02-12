#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include "../../constants.hpp"
#include "../UDP.hpp"
#include "debug.hpp"
#include "../start/start.hpp"
using namespace std;

bool verify_debug(const vector<string>& tokens) {

    if (tokens.size() != 7 || !is_valid_PLID(tokens[1]) || !is_valid_playtime(tokens[2])) {
        cerr << "Invalid command.\n";
        return false;
    }

    return true;
}

int process_debug(const string& input, Player* player, string& request) {

    vector<string> tokens = split_string(input, ' ');
    if (verify_debug(tokens) == false) {
        return 1;   // 1??? pq nao quero repetir prints de erros mas idk
    }
    
    player->player_id = stoi(tokens[1]);
    player->max_playtime = stoi(tokens[2]);
    player->trials = 1;
    
    char buffer[MAX_BUFFER_SIZE];
    int result = snprintf(buffer, sizeof(buffer), "%s %d %s %s %s %s %s\n", DEBUG_CMD, player->player_id, 
    tokens[2].c_str(), tokens[3].c_str(), tokens[4].c_str(), tokens[5].c_str(), tokens[6].c_str());
    
    if (result < 0) {
        return -1;
    }

    request  = string(buffer);
    return 0;
}

void print_debug_response(const vector<string>& tokens, Player* player) {
    
    if (tokens[1] == "NOK") 
        printf("[Error]: debug can´t start as player already has an ongoing game.\n");

    else if (tokens[1] == "ERR")
        printf("[Error]: invalid command.\n");

    else if (tokens[1] == "OK") {
        printf("Debug started.\n");
        player->game_on = true;
    }
}