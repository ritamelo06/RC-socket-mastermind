#ifndef SHOW_TRIALS_HPP
#define SHOW_TRIALS_HPP
#include <string>
#include <vector>
#include "../../constants.hpp"
using namespace std;

int process_show_trials(const string& plid, string& response, int fd, const string& cmd,
                        struct sockaddr_in addr, bool verbose);

string create_show_file(string& path);

int get_remaining_time(string& start_time, string& playtime);

string create_file_lastgame(string& path);

string get_end_status(string& path);

string find_last_game(const string& plid);

string get_game_mode(string& mode);

bool is_FIN(const string& plid);

#endif