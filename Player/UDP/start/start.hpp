#ifndef START_HPP
#define STRAT_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define START_CMD "SNG"
using namespace std;

bool verify_start(const vector<string>& tokens);

int process_start(const string& input, Player* player, string& request);

void print_start_response(const vector<string>& tokens, Player* player);

bool is_valid_PLID(const string& plid);

bool is_valid_playtime(const string& playtime);

#endif