#include <string>
#include <vector>
#include <iostream>
#include "scoreboard.hpp"
#include "../TCP.hpp"
#include "../../UDP/UDP.hpp"
#include "../../constants.hpp"
#include "../TCP.hpp"
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <dirent.h>
using namespace std;
namespace fs = filesystem;

void redimension_vector(SCORELIST& scorelist, int i_file) {
    scorelist.score.resize(i_file + 1);
    scorelist.plid.resize(i_file + 1);
    scorelist.secret_key.resize(i_file + 1);
    scorelist.nr_tries.resize(i_file + 1);
    scorelist.mode.resize(i_file + 1);
}

int find_top10_scores(SCORELIST& scorelist) {
    
    struct dirent **filelist;
    int n_entries, i_file;
    char fname[MAX_BUFFER_SIZE];
    
    n_entries = scandir("GameServer/SCORES", &filelist, NULL, alphasort);
    
    i_file = 0;
    if (n_entries < 0 ) {
        return 0;
    }
    else {
        while(n_entries--) {
           
            if (filelist[n_entries]->d_name[0] != '.') {
                
                sprintf(fname, "GameServer/SCORES/%s", filelist[n_entries]->d_name);
                
                ifstream score_file(fname); 
                if (score_file.is_open()) {
                    
                    string line, score, plid, secret_key, nr_tries, mode;
                    getline(score_file, line);
                    istringstream iss(line);
                    iss >> score >> plid >> secret_key >> nr_tries >> mode;
                    
                    if (scorelist.score.size() <= i_file) 
                        redimension_vector(scorelist, i_file);
                    
                    scorelist.score[i_file] = score;
                    scorelist.plid[i_file] = plid;
                    scorelist.secret_key[i_file] = secret_key;
                    scorelist.nr_tries[i_file] = nr_tries;
                    scorelist.mode[i_file] = mode;

                    score_file.close();
                    ++i_file;
                }   
            }
            free(filelist[n_entries]);
            if (i_file == 10) {
                break;
            }
        }
        free(filelist);
    }
    scorelist.nr_scores = i_file;
    return(i_file);
}

int process_scoreboard(string& response, int fd, const string& cmd, struct sockaddr_in addr, bool verbose) {

    SCORELIST scorelist;
    char response_buffer[MAX_SIZE];

    int n_scores = find_top10_scores(scorelist);
  
    // EMPTY: no scores found
    if (n_scores == 0) {
        int n = snprintf(response_buffer, sizeof(response_buffer), "RSS EMPTY\n");
        if (n < 0) {
            return -1;
        }
        response = string(response_buffer);
        check_verbose(cmd, addr, verbose, response);
        return 0;
    }
    // OK: send scores
    else {
        string score_file_path = create_SB_file(scorelist);
        if (score_file_path == "ERR") {
            return -1;   
        }
        else {
            send_tcp_file(fd, "RSS OK", score_file_path, "", cmd, addr, verbose);
            remove(score_file_path.c_str());  // delete file after sending
        }
        
    }
    return 0;
}

string create_SB_file(SCORELIST& scorelist) {
    
    // create new file

    string sb_file_path = "GameServer/TCP/scoreboard/scoreboard.txt";
    ofstream new_file(sb_file_path);
    if (!new_file.is_open()) {
        return "ERR";
    }

    for(int i = 0; i < scorelist.nr_scores; i++) {
        new_file << scorelist.plid[i] << " " 
                << scorelist.nr_tries[i] << " "
                << scorelist.secret_key[i] << "\n"; 
    }
    new_file.close();
    return sb_file_path;
}