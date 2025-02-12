#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../../constants.hpp"
#include "../../UDP/UDP.hpp"
#include "../TCP.hpp"
#include "show_trials.hpp"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
using namespace std;

int verify_show_trials(const vector<string>& tokens) {
    
    if (tokens.size() != 1) {
        cerr << "[Error]: invalid command.\n";
        return false;
    } else 
        return true;    
}

int process_show_trials(const string& input, Player* player, string& request) {
    
    vector<string> tokens = split_string(input, ' ');

    if (verify_show_trials(tokens) == false) {
        return -1;
    }
    
    char buffer[MAX_BUFFER_SIZE];
    int result = snprintf(buffer, sizeof(buffer), "%s %d\n", SHOW_TRIALS_CMD, player->player_id);
    if (result < 0) {
        return -1;
    }
   
    request = string(buffer);
    return 0;
}


void handle_ST_file (int fd, Player* player, struct addrinfo* res) {

    string status = read_word_from_socket(fd);
    
    if (status.empty()) {
        end_player_app(player, fd, res);
    }
    else if (status == "NOK") {
        cout << "[Error]: unable to show trials.\n";
    }
    else {
        string fname = read_word_from_socket(fd);
        string fsize = read_word_from_socket(fd);

        if (status == "ACT") { 
            display_ST_file(fd, player, res, fname, fsize);
        }
        else if (status == "FIN") {
            display_ST_file(fd, player, res, fname, fsize);
            player->game_on = false; 
        }
        else 
            cerr << "[Error]: invalid response from server.\n"; 
    }
    
}

void display_ST_file(int fd, Player* player, struct addrinfo* res, const string& fname ,const string& fsize) {
    char ch;
    int _fsize = stoi(fsize);
    ssize_t bytes_read = 0;

    string dir = "Player/TCP/show_trials/STcopy/";
    int copy_fd = create_local_copy(fname, fsize, dir, true);
    if (copy_fd < 0) {
        cerr << "[Error]: show_trials copy failed.\n";
        end_player_app(player, fd, res);
    }

    while (bytes_read < _fsize) {     

        ssize_t n = read(fd, &ch, 1); 
        if (n <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cerr << "Timeout reached while waiting for TCP response.\n";
            } else {
                cerr << "[Error]: reading from server failed.\n";
            }
            close(copy_fd);
            end_player_app(player, fd, res);
        }
        cout << ch;
        write(copy_fd, &ch, 1);
        bytes_read = bytes_read + n;    
    }
    cout << endl;
    close(copy_fd);  
}
