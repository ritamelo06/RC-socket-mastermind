#include "quit_exit.hpp"
#include "../UDP.hpp"
#include "../try/try.hpp"
#include "../../constants.hpp"
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int process_quit_exit(vector<string>& tokens, string& response) {
    
    char buffer[MAX_SIZE];
    
    // ERR
    if (tokens.size() != 2 || !is_valid_PLID(tokens[1])) {
        int n = snprintf(buffer, sizeof(buffer), "RQT ERR\n");
        if (n < 0) {
            return -1;
        }
    }
    // NOK
    else if (!has_ongoing_game(tokens[1]) || (has_ongoing_game(tokens[1]) && timeout(tokens[1]))) {
        int n = snprintf(buffer, sizeof(buffer), "RQT NOK\n");
        if (n < 0) {
            return -1;
        }
    }
    // OK: end game
    else {
        time_t current_time = time(NULL);
        string date_game_ended = get_current_date(current_time);
        ifstream infile("GameServer/GAMES/GAME_" + tokens[1] + ".txt");
        string line;
        getline(infile, line);
        istringstream iss(line);
        string status, plid, c1, c2, c3, c4, playtime, date, hours, start_time;
        iss >> plid >> status >> c1 >> c2 >> c3 >> c4 >> playtime >> date >> hours >> start_time;
        
        int n = snprintf(buffer, sizeof(buffer), "RQT OK %s %s %s %s\n", c1.c_str(), c2.c_str(), c3.c_str(), c4.c_str());
        if (n < 0) {
            return -1;
        }
        if (game_over(tokens[1], 'Q', date_game_ended, current_time - stoll(start_time)) < 0) {
            return -1;
        }
    }

    response = string(buffer);
    return 0;
}