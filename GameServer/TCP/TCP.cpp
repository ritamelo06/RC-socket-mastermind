#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include "TCP.hpp"
#include "../UDP/UDP.hpp"
#include "../constants.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
using namespace std;
namespace fs = filesystem;

void read_TCP_socket(int fd, struct sockaddr_in addr, bool verbose) {

    string request = "";
    string cmd = read_word_TCP_socket(fd, SIZE_TCP_CMD);
    
    process_TCP_request(cmd, fd, addr, verbose);    
}


string read_word_TCP_socket(int socket_fd, int size) {

    string word;
    char ch;
  
    while (size+1){

        ssize_t bytes_read = read(socket_fd, &ch, 1);

        if (bytes_read <= 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) 
                cerr << "Timeout reached while waiting for TCP response.\n";       
            else 
                cerr << "[Error]: reading from server failed.\n";  
            return "ERR";
        }
        // parar quando encontra um espaço ou \n
        if (word == "SSB" && ch != '\n') {
            return "INV";
        }
        if (ch == ' ' || ch == '\n') {
            return word;
        }
        size--;
        word += ch;
    }
    return "INV";
}

void process_TCP_request(const string& cmd, int fd, struct sockaddr_in addr , bool verbose) {
    
    string response = "";

    if (cmd == "INV") {
        response = "INV\n";
        check_verbose(cmd, addr, verbose, response);
    }
    else if (cmd == SHOWTRIALS_CMD) {

        string plid = read_word_TCP_socket(fd, SIZE_PLID); 

        if (plid == "ERR") {
            response =  "RST NOK\n";
        }
        else if (process_show_trials(plid, response, fd, cmd, addr, verbose) < 0){
            response = "ERR\n";
            check_verbose(cmd, plid, addr, verbose, response);
        }
    }
    else if (cmd == SCOREBOARD_CMD) {
        if (process_scoreboard(response, fd, cmd, addr, verbose) < 0) {
            response = "ERR\n";
            check_verbose(cmd, addr, verbose, response);
        }
    }
    
    // send response to player
    if (!response.empty()) {
        
        const char* response_buff = response.c_str();
        int size = response.size();
        
        while (size > 0) {
            ssize_t chunk_size = (size > MAX_TCP_RESPONSE) ? MAX_TCP_RESPONSE : size;
            ssize_t n = write(fd, response_buff, chunk_size);
            if (n == -1) {
                cerr << "[Error]: send response to player failed." << endl;
            }
            size -= n;
            response_buff += n;
        }
        return;
    }

}

    
void send_tcp_file(int fd, string response, string file_path, string plid,
                    string cmd, struct sockaddr_in addr, bool verbose) {
    
    // get file size
    uintmax_t fsize = fs::file_size(file_path);
    
    // create file name with timestamp
    time_t current_time = time(NULL);
    string fname;
    if( response == "RST ACT" || response == "RST FIN") {
        fname = "st_" + plid + "_" + to_string(current_time) + ".txt";
    }
    else {
        fname = "sb_" + to_string(current_time) + ".txt"; 
    }

    // open file
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) {
        cerr << "[Error]: unable to open file.\n";
        return;
    }

    // read data from file
    ostringstream file_content_stream;
    file_content_stream << file.rdbuf();
    file.close();
    string file_content = file_content_stream.str();

    // built TCP message
    ostringstream message_stream;
    message_stream << response << " " << fname << " " << fsize << " " << file_content << "\n";

    string message = message_stream.str();

    // verbose
    if (plid.empty()) {
        check_verbose(cmd, addr, verbose, message);
    }
    else {
        check_verbose(cmd, plid, addr, verbose, message);
    }
    
    // write message to socket
    const char* response_buff = message.c_str();
    int size = message.size();
    while (size > 0) {
        
        ssize_t chunk_size = (size > MAX_TCP) ? MAX_TCP : size;
        ssize_t n = write(fd, response_buff, chunk_size);
        if (n == -1) {
            cerr << "[Error]: send response to player failed." << endl;
        }
        size -= n;
        response_buff += n;
    }
}