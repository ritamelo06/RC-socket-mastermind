#ifndef QUIT_EXIT_HPP
#define QUIT_EXIT_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define QUIT_CMD "QUT"
using namespace std;

bool verify_quit_exit(const vector<string>& tokens);

int process_quit_exit(const string& input, Player* player, string& request);

void print_quit_exit_response (const vector<string>& tokens, Player* player, bool is_exit);

#endif