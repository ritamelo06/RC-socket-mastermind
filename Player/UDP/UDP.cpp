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
#include "UDP.hpp"
#include "../constants.hpp"
#include "../TCP/TCP.hpp"
using namespace std;

vector<string> udp_commands = {
    "start",
    "try",
    "quit",
    "exit",
    "debug"
};

bool is_udp_command(const string& command) {
    for(const string& udp_command : udp_commands) {
        if (udp_command == command)
            return true;
    }
    return false;
}


vector<string> split_string(const string& str, char delimiter) {

    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}


void handle_udp_cmd(const string& command, const string& input, Player* player, const string& server_ip, const string& server_port) {

    string request = ""; 
    string plid_player = to_string(player->player_id);
    bool is_exit = false;
    
  
    if(command == "start") {
        string plid_request = split_string(input, ' ')[1];
        
        // já há jogo na app e quem quer começar é um player diferente
        if(player->game_on == true && (plid_player != plid_request)) {
            cerr << "Already started a game on player application.\n";
            return; 
        } 
        else if (process_start(input, player, request) < 0) {
            cerr << "Error processing start command.\n";      
        }

    } else if (command == "try") {
        if (process_try(input, player, request) < 0) 
            cerr << "Error processing try command.\n";
            
    } else if (command == "quit" || command == "exit") {
        if (process_quit_exit(input, player, request) < 0) 
            cerr << "Error processing quit command.\n";
        if (command ==  "exit")
            is_exit = true;
    
    } else if (command == "debug") {
        string plid_request = split_string(input, ' ')[1];

        // já há jogo na app e quem quer começar é um player diferente
        if (player->game_on == true && (plid_player != plid_request)) {
            cerr << "Already started a game on player application.\n";
            return;
        } 
        else if (process_debug(input, player, request) < 0) {
            cerr << "Error processing debug command.\n";
        }   

    } else 
        cerr << "Invalid command.\n";
    
    // send UDP request
    if (!request.empty()) {
        
        if (send_udp_request(request, server_ip, server_port, player, is_exit) == -1) {
            cerr << "Error sending UDP request.\n";
        }
        return;
    }
}


int send_udp_request(const string& request, const string& server_ip, const string& server_port, Player* player, bool is_exit) {

    int sockfd, errcode;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char response_buffer[MAX_RES_SIZE]; 
    struct timeval timeout = {5, 0};
    string udp_response = "";
    
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "[Error]: create UDP socket.\n";
        return -1;
    }

    // set timeout
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        cerr << "[Error]: timeout setting for UDP socket failed.\n";
        close(sockfd);
        return -1;
    }

    // get address info
    memset(&hints,0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo(server_ip.c_str(), server_port.c_str(), &hints, &res);
    if (errcode != 0) {
        cerr << "[Error]: get address info failed.\n";
        close(sockfd);
        return -1;
    }

    ssize_t n = sendto(sockfd, request.c_str(), request.length(), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        cerr << "[Error]: send UDP request failed.\n";
        freeaddrinfo(res);
        close(sockfd);
        return -1;
    }

    addrlen = sizeof(addr);
    memset(response_buffer, 0, sizeof(response_buffer));   // clean buffer before using it ??

    // receive UDP response (3 attempts before shutting down App)
    int no_response = 0;

    while(no_response < 3) {
        n = recvfrom(sockfd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
        if(n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cerr << "Timeout reached while waiting for UDP response.\n";
                no_response++;
                continue;
            } else {
                cerr << "Error receiving message from game server.\n";
                end_player_app(player, sockfd, res);
            }  
        }
        response_buffer[n -1 ] = '\0';
        break;
    }

    udp_response = string(response_buffer);
    display_udp_response(udp_response, player, is_exit);

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}


void display_udp_response(string& udp_reponse, Player* player, bool is_exit) {
    
    vector<string> tokens;
    tokens = split_string(udp_reponse, ' ');
    
    if (tokens[0] == "RSG")
        print_start_response(tokens, player);
          
    else if (tokens[0] == "RTR") 
        print_try_response(tokens, player);

    else if (tokens[0] == "RQT")
        print_quit_exit_response(tokens, player, is_exit);

    else if (tokens[0] == "RDB")
        print_debug_response(tokens, player);
    
    // EXTRA: error from server processing request
    else if (tokens[0] == "ERR")  
        cerr << "[Error]: invalid response from server.\n";

    // EXTRA: poorly formatted requests
    else if (tokens[0] == "INV")
        cerr << "[Error]: invalid command.\n";
        
    else
        cerr << "[Error]: unpredicted response from server\n";
}
