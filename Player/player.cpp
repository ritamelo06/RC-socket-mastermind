#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <getopt.h>
#include <signal.h>
#include "constants.hpp"
#include "UDP/UDP.hpp"
#include "TCP/TCP.hpp"
#include "player.hpp"
using namespace std;

Player* player;
string server_ip = DEFAULT_IP;   
string server_port = DEFAULT_PORT;
int tcp_fd = -1;

// handle Ctrl+C 
void sigint_handler(int sig) {
    
    if (player->game_on) {
        
        string request = "QUT " + to_string(player->player_id) + "\n";
        // send exit request to server
        if (send_udp_request(request, server_ip, server_port, player, true) < 0) {
            cerr << "Error sending exit request.\n";
        }
    }
    else {
        exit(sig);
    }
}


bool verify_port(const string& port_str) {
    if (port_str.empty())
        return false;

    for (char c : port_str) {
        if (!isdigit(c))
            return false;
    }
        
    try {
        int port = stoi(port_str); 
        return port >= 0 && port <= 65535;
    } catch (const exception&) {
        return false;
    }
}


void verify_args(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                server_ip = optarg;  
                break;
            case 'p':
                server_port = optarg;
                if (!verify_port(server_port)) {
                    cerr << "Invalid port: " << server_port << "\n";
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                cerr << "Usage: " << argv[0] << " [-n ip] [-p port]\n";
                exit(EXIT_FAILURE);
        }
    }
}


void process_command(const string& input, Player* player) {
    
    string copy_input = input;
    string command = input.substr(0, input.find(' '));
    
    if (command.empty()) 
        cerr << "Invalid command.\n";

    else if (is_udp_command(command)) 
        handle_udp_cmd(command, copy_input, player, server_ip, server_port);
    
    else if (is_tcp_command(command)) 
        handle_tcp_cmd(command, copy_input, player, server_ip, server_port);
        
    else 
        cerr << "Invalid command.\n";    
}


int main(int argc, char* argv[]) {
    
    verify_args(argc, argv);

    // handle Ctrl+C
    signal(SIGINT, sigint_handler);

    player = new Player();
    
    char input[MAX_COMMAND_SIZE];
    
    while (true) {

        cout << "> ";
        if (fgets(input, MAX_COMMAND_SIZE, stdin) == nullptr) {
            cerr << "Error reading input.\n" << endl;
            break;
        }
        // remove \n from string
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        // verify if input empty
        if (strlen(input) == 0)
            continue;

        process_command(input, player);
    }
    return 0;
}