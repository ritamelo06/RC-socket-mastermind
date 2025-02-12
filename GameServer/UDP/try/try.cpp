#include "../start/start.hpp"
#include "try.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include "../UDP.hpp"
#include "../../constants.hpp"
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <ctime>
using namespace std;
namespace fs = std::filesystem;

vector<char> colours = {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    ORANGE,
    PURPLE
};

bool is_colour(const string& token) {
    if (token.length() != 1)
        return false;

    char C = token[0];
    for(char colour : colours) {
        if (C == colour)
            return true;
    }
    return false;
}

int checkMastermind(const std::string& solution, const std::string& guess) {
    int exactMatches = 0;
    int partialMatches = 0;

    std::vector<bool> matchedInsolution(solution.length(), false);
    std::vector<bool> matchedInGuess(guess.length(), false);

    for (size_t i = 0; i < solution.length(); ++i) {
        if (solution[i] == guess[i]) {
            exactMatches++;
            matchedInsolution[i] = true;
            matchedInGuess[i] = true;
        }
    }

    for (size_t i = 0; i < solution.length(); ++i) {
        if (!matchedInsolution[i]) {
            for (size_t j = 0; j < guess.length(); ++j) {
                if (!matchedInGuess[j] && solution[i] == guess[j]) {
                    partialMatches++;
                    matchedInsolution[i] = true;
                    matchedInGuess[j] = true;
                    break;
                }
            }
        }
    }

    return (exactMatches*10 + partialMatches);
}

bool is_valid_code(const string& c1, const string& c2, const string& c3, const string& c4) {
    if (!is_colour(c1) || !is_colour(c2) || !is_colour(c3) || !is_colour(c4)) {
        return false;
    }
    return true;
}

bool directory_exists(const string& path) {
    return fs::exists(path) && fs::is_directory(path);
}


string calculate_score(int num_tries, double time_taken, double max_time) {

    double time_percentage = (time_taken * 30) / max_time / 10;
    num_tries = ((num_tries - 1) * (90 / 7));
    int score  = 100 - num_tries - time_percentage;
    ostringstream oss;
    oss << setw(3) << setfill('0') << score;
    string result = oss.str();
    return result;
}

string get_current_date(time_t current_time) {
    
    struct tm tm = *localtime(&current_time);
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    string formatted_date = oss.str();
    
    return formatted_date;
}

int game_over(string& plid, char code, string& date_game_ended, time_t game_duration) {
    
    // get current time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    ostringstream formatted_date;
    formatted_date << put_time(&tm, "%Y%m%d_%H%M%S");
    string player_dir_path = "GameServer/GAMES/" + plid;
    
    // change file name from GAME_PLID.txt to YYYYMMDD_HHMMSS_(code).txt
    string new_file_path = "GameServer/GAMES/" + plid + "/" + formatted_date.str() + "_" + code + ".txt"; 
    string old_file_path = "GameServer/GAMES/GAME_" + plid + ".txt";
    
    if (!directory_exists(player_dir_path)) {
        fs::create_directory(player_dir_path);
    }
    fs::rename(old_file_path, new_file_path);

    // write final line in game file YYYYMMDD_HHMMSS_(code).txt
    string line = date_game_ended + " " + to_string(game_duration) + "\n";
    
    int fd = open(new_file_path.c_str(), O_WRONLY | O_APPEND);
    if (fd == -1) {
        cerr << "[Error]: opening game file.\n";
        return -1;
    }
    ssize_t n = write(fd, line.c_str(), line.size());
    if (n == -1) {
        cerr << "[Error]: writing to game file.\n";
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

bool has_been (string& filepath, string& guess) {
    ifstream game_file(filepath);
    bool has_been = false;
    string line;

    while (getline(game_file, line)) { 
        if (line.rfind("T:", 0) == 0) { // Check if the line starts with "T:"
            istringstream iss(line);
            string plid, past_guess, right, half_right, time;

            iss >> plid >> past_guess >> right >> half_right >> time;
            if (past_guess == guess){
                has_been = true;
            }
        }
    }
    return has_been;
}

int get_number_of_trial (string& file_path) {
    
    ifstream game_file(file_path);
    if (!game_file.is_open()) {
        cerr << "[Error]: opening game file.\n";
        return -1;
    }
    string line;
    int number_of_trial = 0;
    while (getline(game_file, line)) { 
        if (!line.empty() && line[0] == 'T') { 
            number_of_trial++;
        }
    }
    game_file.close();
    return number_of_trial;
}

int register_try (string& filepath, string& guess, int result, time_t current_time) {
    
    int nB = result / 10; // Nr of colors in right position
    int nW = result % 10; // Nr of right in wrong place
    string line = "T: " + guess + " " + to_string(nB) + " " + to_string(nW) + " " + to_string(current_time) + "\n";
    
    int fd = open(filepath.c_str(), O_WRONLY | O_APPEND);
    if (fd == -1) {
        cerr << "[Error]: opening game file.\n";
        return -1;
    }
    ssize_t n = write(fd, line.c_str(), line.size());
    if (n == -1) {
        cerr << "[Error]: writing to game file.\n";
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

bool inv_checker(string& file_path, string& guess) {
    
    ifstream game_file(file_path);
    if (!game_file.is_open()) {
        cerr << "[Error]: opening game file.\n";
        return false;
    }
    string line;
    string last_try_line;

    // get last try line in the game file
    while(getline(game_file, line)) {
        if (line.rfind("T:", 0) == 0) {
            last_try_line = line;
        }
    }
    game_file.close();
    
    if (!last_try_line.empty()) {
        istringstream iss(last_try_line);
        string T, past_guess, nB, nW, time; 
        iss >> T >> past_guess >> nB >> nW >> time;
        return past_guess == guess;
    }

    return false;
}

/* 
the trial number nT is the expected value minus 1, and the secret key guess repeats
the one of the previous message (it is a resend) –> in this case the number of trials is not increased;*/
bool OK_exception(string nrTrial_player, string& file_path, int expected_trial, string& guess) {
    return (stoi(nrTrial_player) == (expected_trial - 1) && inv_checker(file_path, guess));    
}

bool is_win (int result) {
    return result == 40;
}

time_t get_game_duration(string& filepath, time_t current_time) {
    
    ifstream game_file(filepath);
    if (!game_file.is_open()) {
        cerr << "[Error]: opening game file.\n";
        return -1;
    }
    string line;
    getline(game_file, line);
    istringstream iss(line);
    string status, plid, c1, c2, c3, c4, playtime, date, hours, start_time;
    iss >> plid >> status >> c1 >> c2 >> c3 >> c4 >> playtime >> date >> hours >> start_time;
    game_file.close();
    return current_time - stoll(start_time);
}

string get_plid(string& filepath) {
    
    ifstream game_file(filepath);
    if (!game_file.is_open()) {
        cerr << "[Error]: opening game file.\n";
        return "";
    }
    string line;
    getline(game_file, line);
    istringstream iss(line);
    string plid;
    iss >> plid;
    game_file.close();
    return plid;
}

int process_try(vector<string>& tokens, string& response) {

    time_t current_time = time(NULL);
    string formatted_date = get_current_date(current_time);
    string guess = tokens[2] + tokens[3] + tokens[4] + tokens[5];
    char buffer[MAX_SIZE];

    // RTR NOK: no ongoing game
    if (!is_valid_PLID(tokens[1]) || !has_ongoing_game(tokens[1])) {
        
        int n = snprintf(buffer, sizeof(buffer), "RTR NOK\n");
        if (n < 0) {
            return -1;
        }
        response = string(buffer);
        return 0;
    }
    
    string file_path = "GameServer/GAMES/GAME_" + tokens[1] + ".txt";
    ifstream game_file(file_path);
    string line;
    getline(game_file, line);
    istringstream iss(line);
    string status, plid, c1, c2, c3, c4, playtime, date, hours, start_time;
    iss >> plid >> status >> c1 >> c2 >> c3 >> c4 >> playtime >> date >> hours >> start_time;
    game_file.close();
    
    time_t game_time = current_time - stoll(start_time);

    int expected_trial = get_number_of_trial(file_path) + 1; // numeber of trials + 1

    // ETM: timeout
    if (stoi(playtime) <= game_time) {
        
        int n = snprintf(buffer, sizeof(buffer), "RTR ETM %s %s %s %s\n",
        c1.c_str(), c2.c_str(), c3.c_str(), c4.c_str());
        if (n < 0) 
            return -1;
        
        if (game_over(plid,'T', formatted_date, game_time) < 0) 
            return -1;
        
    // ERR: invalid command
    } else if (!is_valid_PLID(tokens[1]) || !is_valid_code(tokens[2], tokens[3], tokens[4], tokens[5])) {
        
        int n = snprintf(buffer, sizeof(buffer), "RTR ERR\n");
        if (n < 0) {
            return -1;
        }
    
    // INV 
    } else if ( (stoi(tokens[6]) != expected_trial) || 
    (stoi(tokens[6]) == expected_trial-1 && !inv_checker(file_path, guess))) {
        
        int n = snprintf(buffer, sizeof(buffer), "RTR INV\n");
        if (n < 0) {
            return -1;
        }

    // DUP: repeated guess
    } else if (has_been(file_path, guess)) {

        int n = snprintf(buffer, sizeof(buffer), "RTR DUP\n");
        if (n < 0) {
            return -1;
        }        
     
    // OK ou ENT
    } else {
        int result = checkMastermind(c1 + c2 + c3 + c4, guess);
        
        // exception: expected-1 e DUP guess -> No Trials not increased
        if (OK_exception(tokens[6], file_path, expected_trial, guess)) {
            
            int n = snprintf(buffer, sizeof(buffer), "RTR OK %s %d %d\n",
                             tokens[6].c_str(), result/10, result%10);
            if (n < 0) 
                return -1;
            response = string(buffer);
            return 0;
        }
        
        // Increase No trials -> register in file
        if (register_try(file_path, guess, result, game_time) < 0) {
            cerr << "[Error]: registering try in game file failed.\n";
            return -1;
        }
        
        // ENT: last try and failed
        if (expected_trial == 8 && !is_win(result)) {
            
            int n = snprintf(buffer, sizeof(buffer), "RTR ENT %s %s %s %s\n",
                             c1.c_str(), c2.c_str(), c3.c_str(), c4.c_str());
            if (n < 0) 
                return -1;
            
            if (game_over(plid, 'F', formatted_date, game_time) < 0) 
                return -1;
            
        // OK: normal try
        } else {
            int n = snprintf(buffer, sizeof(buffer), "RTR OK %d %d %d\n",
                             expected_trial, result/10, result%10);
            if (n < 0) {
                return -1;
            }
            if (is_win(result)) {
                if (game_over(plid, 'W', formatted_date, game_time) < 0 )
                    return -1;

                string score = calculate_score(expected_trial, game_time, stoi(playtime));
                create_score_file(score, plid, current_time, expected_trial, c1+c2+c3+c4, status); 
            }
        }
    }
  
    response = string(buffer);
    return 0;
}

void create_score_file(string& score, string& plid, time_t date_ended, int nr_tries, const string& code, string& mode) {
    
    // get date and time game ended
    struct tm tm = *localtime(&date_ended);
    ostringstream oss;
    oss << put_time(&tm, "%Y%m%d_%H%M%S");
    string formatted_date = oss.str();

    // create score file path
    string score_file_path = "GameServer/SCORES/" + score + "_" + plid + "_" + formatted_date.c_str() + ".txt";

    int fd = open(score_file_path.c_str(), O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        cerr << "[Error]: opening score file.\n";
        return;
    }
    string line = score + " " + plid + " " + code + " " + to_string(nr_tries) + " " + mode + "\n";
    ssize_t n = write(fd, line.c_str(), line.size());
    if (n == -1) {
        cerr << "[Error]: writing to score file.\n";
        close(fd);
        return;
    }
    close(fd);
}