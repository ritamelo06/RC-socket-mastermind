#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define DEBUG_CMD "DBG"
#define MODE_DEBUG "D"

bool verify_debug(const vector<string>& tokens);

int process_debug(const string& input, Player* player, string& request);

void print_debug_response(const vector<string>& tokens, Player* player);

#endif