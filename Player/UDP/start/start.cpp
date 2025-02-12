#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include "../../constants.hpp"
#include "../UDP.hpp"
#include "start.hpp"
using namespace std;


bool is_valid_PLID(const string& plid) {
    
    if (plid.length() != SIZE_PLID)
        return false;
    for(char c : plid) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

bool is_valid_playtime(const string& playtime) {
     
    for(char c : playtime) {
        if (!isdigit(c))
            return false;
    }

    return stoi(playtime) <= MAX_PLAYTIME && stoi(playtime) > 0;
}

bool verify_start(const vector<string>& tokens) {
    
    if (tokens.size() != 3 || !is_valid_PLID(tokens[1]) || !is_valid_playtime(tokens[2])) {
        cerr << "Invalid command.\n";
        return false;
    } else {
        return true;
    }      
}

int process_start(const string& input, Player* player, string& request) {
    
    vector<string> tokens = split_string(input, ' ');
    
    if (verify_start(tokens) == false) {
        return 1;   // 1??? pq nao quero repetir prints de erros mas idk
    }
    
    if (player == nullptr) {
        cerr << "Player not initialized.\n";
        return -1;
    }
    
    player->player_id = stoi(tokens[1]);
    player->max_playtime = stoi(tokens[2]);
    player->trials = 1;

    char buffer[MAX_BUFFER_SIZE];
    int result = snprintf(buffer, sizeof(buffer), "%s %d %s\n", START_CMD, player->player_id, tokens[2].c_str());
    if (result < 0) {
        return -1;
    }
   
    // Transferir o buffer formatado para a string de saída
    request = string(buffer);
    
    return 0;
}

void print_start_response(const vector<string>& tokens, Player* player) {
    
    if (tokens[1] == "OK") {
        cout << "Game started!\n";
        player->game_on = true;
    }    
    else if (tokens[1] == "NOK") 
        cout << "[Error]: player already has an ongoing game.\n";
        
    else if (tokens[1] == "ERR") 
        cout << "[Error]: invalid command.\n";    
}


