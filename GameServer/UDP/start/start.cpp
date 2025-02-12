#include <string>
#include <vector>
#include <iostream>
#include "start.hpp"
#include "../UDP.hpp"
#include "../../constants.hpp"
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
using namespace std;
namespace fs = filesystem;

vector<char> colors = {RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE};

bool is_valid_PLID(const string& plid) {
    
    if (plid.length() != SIZE_PLID)
        return false;
    for(char c : plid) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

bool is_valid_playtime(const string& playtime) {
    
    for(char c : playtime) {
        if (!isdigit(c)) {
            return false;
        }
    }
    if (stoi(playtime) > MAX_PLAYTIME || stoi(playtime) <= 0)
        return false;  

    return true;
}


bool has_ongoing_game(const string& plid) { 
    ifstream game_file("GameServer/GAMES/GAME_" + plid + ".txt");
    return game_file.good();
}

int generate_solution() {
    srand(time(0)); // Seed the random number generator
    int col1 = rand()%6;
    int col2 = rand()%6;
    int col3 = rand()%6;
    int col4 = rand()%6;
    return col1*1000 + col2*100 + col3*10 + col4;
}

int create_plid_file(const string& plid, const string& playtime, int solution) {
    
    string file_path = "GameServer/GAMES/GAME_" + plid + ".txt";
    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0644);
    
    if (fd == -1) {
        cerr << "[Error]: creating game file.\n";
        return -1;
    }

    char col1 = colors[solution / 1000];
    char col2 = colors[(solution / 100) % 10];
    char col3 = colors[(solution / 10) % 10];
    char col4 = colors[solution % 10];
    
    time_t t = time(NULL); 
    struct tm tm = *localtime(&t);
    ostringstream formatted_time;
    formatted_time << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    // weite firt line of file GAME_PLID.txt
    string line = plid + " " + MODE_PLAY + " " + col1 + " " + col2 + " " + col3 + " " + 
    col4 + " " + playtime + " " + formatted_time.str() + " " + to_string(t) + "\n";

    ssize_t bytes_writen = write(fd, line.c_str(), line.size());
    
    if (bytes_writen == -1) {
        cerr << "[Error]: writing to game file.\n";
        close(fd);
        return -1;
    
    }
    close(fd);
    return 0;
}

bool timeout(string plid) {
    
    // read info from file
    string game_path = "GameServer/GAMES/GAME_" + plid + ".txt";
    ifstream game_file(game_path);
    string line;
    getline(game_file, line);
    istringstream iss(line);
    string status, PLID, c1, c2, c3, c4, playtime, date, hours, start_time;
    iss >> PLID >> status >> c1 >> c2 >> c3 >> c4 >> playtime >> date >> hours >> start_time;
    game_file.close();

    // calculate game time
    time_t current_time = time(NULL);
    time_t game_time = current_time - stoll(start_time);
    bool timeout = stoi(playtime) <= game_time;
    
    if (timeout) {
        string formatted_date = get_current_date(current_time);
        game_over(plid,'T', formatted_date, game_time);
    }
    return timeout;
}

int process_start(vector<string>& tokens, string& response) {
    
    char buffer[MAX_SIZE];
   

    // ERR
    if (tokens.size() != 3 || !is_valid_PLID(tokens[1]) || !is_valid_playtime(tokens[2])){
        int n = snprintf(buffer, sizeof(buffer), "RSG ERR\n");
        if (n < 0) {
            return -1;
        }

    // OK: no ongoing game or had ongoing game but reached timeout
    } else if (!has_ongoing_game(tokens[1]) || (has_ongoing_game(tokens[1]) && timeout(tokens[1]))) {
        
        int n = snprintf(buffer, sizeof(buffer), "RSG OK\n");
        if (n < 0) { 
            return -1;
        }
        int solution = generate_solution();
        
        if (create_plid_file(tokens[1], tokens[2], solution) < 0 ) {
            return -1;
        }
    }

    // NOK: ongoing game and no timeout
    else if (has_ongoing_game(tokens[1]) && !timeout(tokens[1])) {
        
        int n = snprintf(buffer, sizeof(buffer), "RSG NOK\n");
        if (n < 0) {
            return -1;
        }
    }
    
    response = string(buffer);
    return 0;
}

