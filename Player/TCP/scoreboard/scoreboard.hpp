#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define SCOREBOARD_CMD "SSB"
using namespace std;

int verify_scoreboard(const vector<string>& tokens);

int process_scoreboard(const string& input, string& request);

void handle_SB_file (int fd, Player* player, struct addrinfo* res);

void display_SB_file(int fd, Player* player, struct addrinfo* res, const string& fname ,const string& fsize);

#endif


