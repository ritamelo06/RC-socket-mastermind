#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include "../../constants.hpp"
#include "../UDP.hpp"
#include "try.hpp"
using namespace std;

vector<string> colours = {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    ORANGE,
    PURPLE
};

bool is_colour(const string& token) {
    
    if (token.length() != 1)
        return false;

    for(const string& c : colours) {
        if (token == c)
            return true;
    }
    return false;
}

bool verify_try(const vector<string>& tokens) {
    if (tokens.size() != 5) {
        cerr << "Invalid command.\n";
        return false;
    }
    else 
        return true;
}

int process_try (const string& input, Player* player, string& request) {
    vector<string> tokens = split_string(input, ' ');

    tokens = split_string(input, ' ');
    if(verify_try(tokens) == false) {
        return 1;    // retornar 1? se retornar -1 dou print de dois erros
    }

    if (player == nullptr) {
        cerr << "Player not initialized.\n";
        return -1;
    }
 
    char buffer[MAX_BUFFER_SIZE];
    int n = snprintf(buffer, sizeof(buffer), "%s %d %s %s %s %s %d\n", TRY_CMD, player->player_id,
    tokens[1].c_str(), tokens[2].c_str(), tokens[3].c_str(), tokens[4].c_str(), player->trials);
    if (n < 0) {
        return -1;
    }
   
    // Transferir o buffer formatado para a string de saída
    request = string(buffer);

    return 0;
}

void print_try_response(const vector<string>& tokens, Player* player) {
   
    if (tokens[1] == "OK") {
        
        player->trials++;
        printf("Trial number: %s\n", tokens[2].c_str());
        printf("Nr of guesses correct in colour and position: %s\n", tokens[3].c_str());
        printf("Nr of guesses correct in colour but not in position: %s\n", tokens[4].c_str());
        
        if (tokens[3] == "4") {
            printf("Congratulations! You guessed the secret code!\n");
            player->game_on = false;
        }
    }
    else if (tokens[1] == "DUP") 
        printf("Already guessed!\n");
    
    else if (tokens[1] == "INV") {
        printf("[Error]: invalid number of trials.\n" );
    }
    else if (tokens[1] == "NOK") 
        printf("[Error]: player doesn't have an ongoing game.\n");

    else if (tokens[1] == "ENT") {
        printf("Game Over! No more attempts available.\n");
        printf("Secret key: %s %s %s %s\n", tokens[2].c_str(),
        tokens[3].c_str(), tokens[4].c_str(), tokens[5].c_str());
        player->game_on = false;
    }
    else if (tokens[1] == "ETM") {
        printf("Game Over! Maximum playtime reached.\n");
        printf("Secret key: %s %s %s %s\n", tokens[2].c_str(),
        tokens[3].c_str(), tokens[4].c_str(), tokens[5].c_str());
        player->game_on = false;
    }
    else if (tokens[1] == "ERR") 
        printf("[Error]: invalid command.\n");       
}