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
#include "scoreboard.hpp"
using namespace std;


int verify_scoreboard(const vector<string>& tokens) {
    
    if (tokens.size() != 1) {
        cerr << "[Error]: invalid command.\n";
        return false;
    } else 
        return true;    
}


int process_scoreboard(const string& input, string& request) {
    
    vector<string> tokens = split_string(input, ' ');

    if (verify_scoreboard(tokens) == false) {
        return -1;
    }
    
    char buffer[MAX_BUFFER_SIZE];
    int result = snprintf(buffer, sizeof(buffer), "%s\n", SCOREBOARD_CMD); 
    if (result < 0) {
        return -1;
    }

    request = string(buffer);  
    return 0;
}


void handle_SB_file (int fd, Player* player, addrinfo* res) {

    string status = read_word_from_socket(fd);

    if (status.empty()) {
        end_player_app(player, fd, res);
    }
    else if (status == "EMPTY") {
        cout << "Scoreboard is empty.\n";
    }
    else if (status == "OK") {
        string fname = read_word_from_socket(fd);
        string fsize = read_word_from_socket(fd);
        display_SB_file(fd, player, res, fname, fsize);
    }
    else 
        cerr << "[Error]: invalid response from server.\n";
}

void display_SB_file(int fd, Player* player, struct addrinfo* res, const string& fname ,const string& fsize) {
    char ch;
    int _fsize = stoi(fsize);
    ssize_t bytes_read = 0;

    string dir = "Player/TCP/scoreboard/SCOREBOARDcopy/";
    int copy_fd = create_local_copy(fname, fsize, dir, false );
    if (copy_fd == -1) {
        cerr << "[Error]: unable to create local copy of scoreboard.\n";
        end_player_app(player, fd, res);
    }

    while (bytes_read < _fsize) {

        ssize_t n = read(fd, &ch, 1);
        if ( n <= 0) {
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
