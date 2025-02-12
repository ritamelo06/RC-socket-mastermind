#include <string>
#include <vector>
#include <iostream>
#include "../start/start.hpp"
#include "../try/try.hpp"
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

int create_plid_file(const string& plid, const string& playtime, string& solution) {
    
    string file_path = "GameServer/GAMES/GAME_" + plid + ".txt";
    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0644);
    
    if (fd == -1) {
        cerr << "[Error]: creating game file.\n";
        return -1;
    }
    
    time_t t = time(NULL); 
    struct tm tm = *localtime(&t);
    ostringstream formatted_time;
    formatted_time << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    // weite firt line of file GAME_PLID.txt
    string line = plid + " " + MODE_DEBUG + " " + solution[0] + " " + solution[1] + " " + solution[2] + " " + 
    solution[3] + " " + playtime + " " + formatted_time.str() + " " + to_string(t) + "\n";

    ssize_t bytes_writen = write(fd, line.c_str(), line.size());
    if (bytes_writen == -1) {
        cerr << "[Error]: writing to game file.\n";
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

bool verify_debug(const vector<string>& tokens) {
    
    if (tokens.size() != 7 || !is_valid_PLID(tokens[1]) || !is_valid_playtime(tokens[2])
    || !is_valid_code(tokens[3], tokens[4], tokens[5], tokens[6])) {
        
        return false;
    }
    return true;
}


int process_debug(vector<string>& tokens, string& response) {
        
    char buffer[MAX_SIZE];
    string solution;
    
    // ERR
    if (!verify_debug(tokens)) {
        int n = snprintf(buffer, sizeof(buffer), "RDB ERR\n");
        if (n < 0) {
            return -1;
        }

    // NOK: ongoing game and no timeout
    } else if (has_ongoing_game(tokens[1]) && !timeout(tokens[1])) {
        int n = snprintf(buffer, sizeof(buffer), "RDB NOK\n");
        if (n < 0) {
            return -1;
        }
    
    // OK: no ongoing game or had ongoing game but reached timeout
    } else if (!has_ongoing_game(tokens[1]) || (has_ongoing_game(tokens[1]) && timeout(tokens[1]))){
        
        int n = snprintf(buffer, sizeof(buffer), "RDB OK\n");
        if (n < 0) { 
            return -1;
        }
        solution = tokens[3] + tokens[4] + tokens[5] + tokens[6];
        
        if (create_plid_file(tokens[1], tokens[2], solution) < 0 ) {
            return -1;
        }
    }
    
    response = string(buffer);
    return 0;
}

