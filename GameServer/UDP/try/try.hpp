#ifndef TRY_HPP
#define TRY_HPP
#include <string>
#include <vector>
#include "../../constants.hpp"
using namespace std;

int process_try(vector<string>& tokens, string& response);

bool is_valid_code(const string& c1, const string& c2, const string& c3, const string& c4);

bool directory_exists(const string& path);

int game_over(string& plid, char code, string& date_game_ended, time_t game_duration);

bool is_colour(const string& c);

int checkMastermind(const string& solution, const string& guess);

bool has_been(string& file_path,string& guess);

int get_number_of_trial(const string& file_path);

bool has_ongoing_game(const string& plid);

string get_current_date(time_t current_time);

bool OK_exception(string nrTrial_player, string& file_path, int expected_trial, string& guess);

bool inv_checker(string& file_path, string& guess);

bool is_win(int result);

time_t get_game_duration(string& filepath, time_t current_time);

string get_plid(string& filepath);

void create_score_file(string& score, string& plid, time_t date_ended, int nr_tries, const string& code, string& status);

string calculate_score(int num_tries, double time_taken, double max_time);


#endif