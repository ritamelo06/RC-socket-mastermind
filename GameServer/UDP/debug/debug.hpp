#ifndef DEBUG_HPP
#define DEBUG_HPP
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

int create_plid_file(const string& plid, const string& playtime, string& solution);

int process_debug(vector<string>& tokens, string& response);

#endif