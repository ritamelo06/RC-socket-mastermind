#ifndef START_HPP
#define START_HPP
#include <string>
#include <vector>
#include "../../constants.hpp"
#define RED 'R'
#define GREEN 'G'
#define BLUE 'B'
#define YELLOW 'Y'
#define ORANGE 'O'
#define PURPLE 'P'
using namespace std;

int process_start(vector<string>& tokens, string& response);

int generate_solution();

bool has_ongoing_game(const string& plid);

bool is_valid_playtime(const string& playtime);

bool is_valid_PLID(const string& plid);

int create_plid_file(const string& plid, const string& playtime, int solution);

bool timeout(string plid);

#endif