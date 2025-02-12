#include <string>
#include <vector>
#include <iostream>
#include "show_trials.hpp"
#include "../TCP.hpp"
#include "../../UDP/UDP.hpp"
#include "../../constants.hpp"
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <dirent.h>
#include <algorithm>

using namespace std;
namespace fs = filesystem;


int get_remaining_time(string& start_time, string& playtime) {
    
    time_t current_time = time(NULL);
    time_t elapsed_time = current_time - stoll(start_time);
    
    return stoi(playtime) - elapsed_time;
}

string create_show_file(const string& path) {
    
    ifstream game_file(path); 
    if (!game_file.is_open()) {
        return "ERR";
    }
    // read game info
    string line;
    string status, plid, c1, c2, c3, c4, playtime, date, hours, start_time;
    vector<string> trials;
    while (getline(game_file, line)) {
        if (line.rfind("T:", 0) == 0) {
            trials.push_back(line);
        } 
        else {
            istringstream iss(line);
            iss >> plid >> status >> c1 >> c2 >> c3 >> c4 >>
            playtime >> date >> hours >> start_time;
        }
    }
    game_file.close();

    int remaining_time = get_remaining_time(start_time, playtime);

    // create new file
    string show_file_path = "GameServer/GAMES/show_trials_" + plid + ".txt";
    ofstream new_file(show_file_path);
    if (!new_file.is_open()) {
        return "ERR";
    }
    
    // write game info to new file
    new_file << "     Active game found for player " << plid << "\n";
    new_file << "Game initiated: " << date << " " << hours << " with " << playtime << " seconds to be completed\n\n";
    new_file << "     --- Transactions found: " << trials.size() << " ---\n";

    for (const string& trial : trials) {
        istringstream iss(trial);
        string T, guess, nB, nW, time;
        iss >> T >> guess >> nB >> nW >> time;
        new_file << "Trial: " << guess << ", nB: " << nB << ", nW: " << nW << " at " << time << "s\n";
    }
    new_file << "\n -- " << remaining_time << " seconds remaining to be completed --\n";
    new_file.close();

    return show_file_path;
}

string find_last_game(const string& PLID) {
    string dirname = "GameServer/GAMES/" + PLID + "/";
    vector<string> filelist;
    string fname;
    DIR* dir = opendir(dirname.c_str());

    if (!dir) {
        return "RSR NOK";
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') {
            filelist.push_back(entry->d_name);
        }
    }
    closedir(dir);

    if (filelist.empty()) {
        return "RSR NOK";
    }

    // Sort filelist alphabetically
    sort(filelist.begin(), filelist.end());

    // Get the last file in the sorted list
    fname = dirname + filelist.back();

    return fname;
}

string get_end_status(string& path) {
    
    string code = path.substr(40,1);
    
    if (code == "Q") 
        return "QUIT";
    else if (code == "W") 
        return "WIN";
    else if (code == "F") 
        return "FAIL";
    else if (code == "T") 
        return "TIMEOUT";
    else 
        return "";   
}

string get_game_mode (string& mode) {
        
    if (mode == "P") 
        return "PLAY";
    else if (mode == "D") 
        return "DEBUG";
    else 
        return "";
}

string create_file_lastgame(string& path) {
    
    ifstream game_file(path); 
    if (!game_file.is_open()) {
        return "ERR";
    }
    
    string line, status, plid, c1, c2, c3, c4, playtime, date, 
    hours, start_time, date_ended, hours_ended, duration;
    vector<string> trials;

    // read first line
    getline(game_file, line);
    istringstream iss(line);
    iss >> plid >> status >> c1 >> c2 >> c3 >> c4 >>
    playtime >> date >> hours >> start_time;
    
    // read trials
    while(getline(game_file, line)) {
        if (line.rfind("T:", 0) == 0) {
            trials.push_back(line);
        } 
        else {
            // read last line
            istringstream iss(line);
            iss >> date_ended >> hours_ended >> duration;
        }
    }
    game_file.close();
    string game_mode = get_game_mode(status);
    string end_status = get_end_status(path);

    // create new file
    string show_file_path = "GameServer/GAMES/show_trials_" + plid + ".txt";
    ofstream new_file(show_file_path);
    if (!new_file.is_open()) {
        return "ERR";
    }

    // write game info to new file
    new_file << "     Last finalized game for player " << plid << "\n";
    new_file << "Game initiated: " << date << " " << hours << " with " << playtime << " seconds to be completed\n";
    new_file << "Mode: " << game_mode << "  Secret code: " << c1 << c2 << c3 << c4 << "\n\n";
    new_file << "     --- Transactions found: " << trials.size() << " ---\n";

    for (const string& trial : trials) {
        istringstream iss(trial);
        string T, guess, nB, nW, time;
        iss >> T >> guess >> nB >> nW >> time;
        new_file << "Trial: " << guess << ", nB: " << nB << ", nW: " << nW << " at " << time << "s\n";
    }
    new_file << "     Termination: " << end_status << " at " << date_ended << " " << hours_ended << ", Duration: " << duration <<"\n"; 
    new_file.close();

    return show_file_path;
}

int process_show_trials(const string& plid, string& response, int fd, const string& cmd,
                        struct sockaddr_in addr, bool verbose) {
    
    char response_buffer[MAX_SIZE];
    memset(response_buffer, 0, sizeof(response_buffer)); // clean buffer

    // check if command is valid
    if (!is_valid_PLID(plid)) {
        int n = snprintf(response_buffer, sizeof(response_buffer), "RST NOK\n");
        if (n < 0) {
            return -1;
        }
        response = string(response_buffer);
        check_verbose(cmd, plid, addr, verbose, response);
        return 0;
    }

    // ACK: player has game
    if (has_ongoing_game(plid) && !timeout(plid)) {
        
        string show_file_path = create_show_file("GameServer/GAMES/GAME_" + plid + ".txt");
        if (show_file_path == "ERR") {
            return -1;
        }
        else {
            send_tcp_file(fd, "RST ACT", show_file_path, plid, cmd, addr, verbose);
            remove(show_file_path.c_str()); // apagar show file
        }
    }
    // FIN: player has last game and no ongoing game/timeout
    else if (is_FIN(plid)) {
        
        string last_game_path = find_last_game(plid);
        
        string show_file_path = create_file_lastgame(last_game_path);
        if (show_file_path == "ERR") {
            return -1;
        }
        else {
            send_tcp_file(fd, "RST FIN", show_file_path, plid, cmd, addr, verbose);
            remove(show_file_path.c_str()); // apagar show file
        }
    }
    // NOK: player has no games (or Errors)
    else {
        int n = snprintf(response_buffer, sizeof(response_buffer), "RST NOK\n");
        if (n < 0) {
            return -1;
        }
        response = string(response_buffer);
        check_verbose(cmd, plid, addr, verbose, response);
        return 0;
    }
    return 0;
}


bool is_FIN(const string& plid) {

    return (!has_ongoing_game(plid) && directory_exists("GameServer/GAMES/" + plid)) 
        || (has_ongoing_game(plid) && timeout(plid) && directory_exists("GameServer/GAMES/" + plid));
}