#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "TCP.hpp"
#include "../constants.hpp"
#include <dirent.h>
#include <fcntl.h>
using namespace std;
extern int tcp_fd;


vector<string> tcp_commands = {
    "scoreboard",   
    "sb",
    "show_trials",
    "st",
};

bool is_tcp_command(const string& command) {
    for(const string& tcp_command : tcp_commands) {
        if (tcp_command == command)
            return true;
    }
    return false;
}

void handle_tcp_cmd(const string& command, const string& input, Player* player, 
                    const string& server_ip, const string& server_port) {
    
    string request = "";

    if (command == "scoreboard" || command == "sb") {
        if (process_scoreboard(input, request) < 0) {
            cerr << "Error processing scoreboard command.\n";
        }

    } else if (command == "show_trials" || command == "st") {
        if (process_show_trials(input, player, request) < 0) {
            cerr << "Error processing show_trials command.\n";
        }

    } else {
        cerr << "Invalid command.\n";
    }

    if (!request.empty()) {
        
        if (send_tcp_request(request, server_ip, server_port, player) == -1) {
            cerr << "Error sending TCP request.\n";
        }
        return;
    }
}

void end_player_app (Player* player, int fd, struct addrinfo* res) {
    cerr << "Exiting Player Application.\n";
    player->game_on = false;
    freeaddrinfo(res);
    close(fd);
    exit(EXIT_FAILURE);
}

int send_tcp_request (const string& request, const string& server_ip,
                        const string& server_port, Player* player) {
    
    struct addrinfo hints, *res;

    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        cerr << "[Error]: create TCP socket failed.\n";
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // set timeout
    if (setsockopt(tcp_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        cerr << "[Error]: setsockopt failed.\n";
        close(tcp_fd);
        tcp_fd = -1;
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    if (getaddrinfo(server_ip.c_str(), server_port.c_str(), &hints, &res) != 0) {
        cerr << "[Error]: getaddrinfo failed.\n";
        freeaddrinfo(res);
        return -1;
    }

    if (connect(tcp_fd, res->ai_addr, res->ai_addrlen) == -1) {
        cerr << "[Error]: connection to server failed.\n";
        end_player_app(player, tcp_fd, res);
    }

    const char* req_buff = request.c_str(); 
    int size = request.length();
    while(size > 0) {
        
        ssize_t chunk_size = (size > MAX_TCP_REQ) ? MAX_TCP_REQ : size;
        
        ssize_t n = write(tcp_fd, req_buff, chunk_size);
        if (n == -1) {
            cerr << "[Error]: writing to server failed.\n";
            freeaddrinfo(res);
            return -1;
        }
        size -= n;
        req_buff += n;
    }        

    display_TCP_response(tcp_fd, player, res);

    freeaddrinfo(res);
    close(tcp_fd); 
    tcp_fd = -1;
    return 0;
}


string read_word_from_socket(int socket_fd) {
    string word;
    char ch;

    while (true) {
        
        ssize_t bytes_read = read(socket_fd, &ch, 1);

        if (bytes_read <= 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) 
                cerr << "Timeout reached while waiting for TCP response.\n";       
            else 
                cerr << "[Error]: reading from server failed.\n";  
            return "";
        }
        // parar quando encontra um espaço ou \n
        if (ch == ' ' || ch == '\n') {
            break;
        }

        word += ch;
    }

    return word;
}

void display_TCP_response(int fd, Player* player, struct addrinfo* res) {
    
    string cmd = read_word_from_socket(fd);
   
    if (cmd.empty()) 
        end_player_app(player, fd, res);
    
    else if (cmd == "RST") 
        handle_ST_file(fd, player, res);   // show_trials
       
    else if (cmd == "RSS")
        handle_SB_file(fd, player, res);   // scoreboard
    
    else if (cmd == "ERR")
        cerr << "[Error]: invalid response from server.\n";  
        
    else if (cmd == "INV")
        cerr << "[Error]: invalid command.\n";

    else
        cerr << "[Error]: unpredicted response from server\n";
}


int create_local_copy(const string& fname, const string& fsize, const string& dir, bool is_show_trials) {
    
    string full_path = dir + fname;
    
    int fd = open(full_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        cerr << "[Error]: opening file failed.\n";
        return -1;
    }
    if (is_show_trials) {
        cout << "Local copy of show trials created" << endl;
        cout << "  - File path: Player/TCP/show_trials/STcopy" << endl;
    } else {
        cout << "Local copy of scoreboard created" << endl;
        cout << "  - File path: Player/TCP/scoreboard/SCOREBOARcopy" << endl;
    }
    cout << "  - File name: " << fname << endl;
    cout << "  - File size: " << fsize << endl;
    cout << endl;
    return fd;
}

