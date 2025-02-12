#ifndef TRY_HPP
#define TRY_HPP

#include "../../player.hpp"
#include "../../constants.hpp"
#include <string>
#include <vector>
#define RED "R"
#define GREEN "G"
#define BLUE "B"
#define YELLOW "Y"
#define ORANGE "O"
#define PURPLE "P"
#define TRY_CMD "TRY"

bool is_colour(const string& token);

bool verify_try(const vector<string>& tokens);

int process_try (const string& input, Player* player, string& request);

void print_try_response(const vector<string>& tokens, Player* player);

#endif