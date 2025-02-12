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
using namespace std;

vector<string> split_string(const string& str, char delimiter) {

    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void read_UDP_socket(int fd, bool verbose) {
    
    char buffer[MAX_SIZE];
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    ssize_t n;
     
    memset(buffer, 0, sizeof(buffer)); // clean buffer before using it ??

    n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        cerr << "[Error]: recvfrom failed while reading UDP socket." << endl;
        return;
    }
    buffer[n - 1] = '\0';
         
    string request = string(buffer);
    process_UDP_request(request, fd, addr, addrlen, verbose);  
} 


void process_UDP_request(const string& request, int fd, struct sockaddr_in addr, socklen_t addrlen, bool verbose) {

    string response = "";
    vector<string> tokens = split_string(request, ' ');

    if (tokens[0] == START_CMD) {
        if (process_start(tokens, response) < 0) 
            response = "ERR\n";
    }
    else if (tokens[0] == TRY_CMD) {
        if (process_try(tokens, response) < 0) 
            response = "ERR\n";
    }
    else if (tokens[0] == QUIT_EXIT_CMD) {
        if (process_quit_exit(tokens, response) < 0)
            response = "ERR\n";
    }
    else if (tokens[0] == DEBUG_CMD) {
        if (process_debug(tokens, response) < 0)
            response = "ERR\n";
    }
    else {
        response = "INV\n";
    }
    
    // verbose
    if (tokens[1].empty()) 
        check_verbose(tokens[0], addr, verbose, response);
    else 
        check_verbose(tokens[0], tokens[1], addr, verbose, response); 
    
    // send response to player
    if (!response.empty()) {
        
        ssize_t n = sendto(fd, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr*)&addr, addrlen);
        if (n == -1) {
            cerr << "[Error]: send response to player failed." << endl;
        }
        return;
    }
}

void check_verbose(const string& cmd, const string& plid, struct sockaddr_in addr, bool verbose, string& response) {
    if (verbose) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "------------------------------------------------------------" << endl;
        cout << "Request information:" <<endl;
        cout << "   Command - " << cmd << endl;
        cout << "   Player ID - " << plid << endl;
        cout << "   Origin IP - " << ip << endl;
        cout << "   Origin port - " << ntohs(addr.sin_port) << endl << endl;

        if (!response.empty()) {
            cout << "Response sent to player:" << endl;
            cout << response << endl;
        }
    }
}

void check_verbose(const string& cmd, struct sockaddr_in addr, bool verbose, string& response) {
    if (verbose) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "------------------------------------------------------------" << endl;
        cout << "Request information:" <<endl;
        cout << "   Command - " << cmd << endl;
        cout << "   Origin IP - " << ip << endl;
        cout << "   Origin port - " << ntohs(addr.sin_port) << endl << endl;

        if (!response.empty()) {
            cout << "Response sent to player:" << endl;
            cout << response << endl;
        }
    }
}

