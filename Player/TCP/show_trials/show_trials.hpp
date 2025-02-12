#ifndef SHOW_TRIALS_HPP
#define SHOW_TRIALS_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define SHOW_TRIALS_CMD "STR"
using namespace std;

int verify_show_trials(const vector<string>& tokens);

int process_show_trials(const string& input, Player* player, string& request);

void handle_ST_file(int fd, Player* player, struct addrinfo* res);

void display_ST_file(int fd, Player* player, struct addrinfo* res, const string& fname, const string& fsize);

#endif


