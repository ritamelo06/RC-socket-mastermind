#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <cctype>
#include <memory>
#include "constants.hpp"
#include "UDP/UDP.hpp"
#include "TCP/TCP.hpp"
#include <filesystem>
#include <ctime>
using namespace std;
namespace fs = filesystem;
extern int errno; // está no pp do stor

bool verbose = false;
string server_port = DEFAULT_PORT;

int udp_sock = -1;
int tcp_sock = -1;

void setup_signal_handler(int signal, void (*handler)(int)) {

    struct sigaction act;
    act.sa_handler = handler;
    if (sigaction(signal, &act, nullptr) == -1) {
        cerr << "[Error]: sigaction failed for signal " << signal << endl;
        exit(EXIT_FAILURE);
    }
}

// handle Ctrl+C
void handle_sigint(int signum) {

    time_t current_time = time(NULL);
    string date_game_ended = get_current_date(current_time);
    
    // move ongoin games to GAMES folder
    try {
        for (const auto& entry : fs::directory_iterator("GameServer/GAMES")) {
            
            if (fs::is_regular_file(entry.path())) {
                string filepath = entry.path().string();
                
                if (filepath.rfind("GameServer/GAMES/GAME_", 0) == 0) {
                    
                    string plid = get_plid(filepath);            
                    time_t game_duration = get_game_duration(filepath, current_time);
                    game_over(plid, 'Q', date_game_ended, game_duration);
                }
                else {
                    remove(filepath.c_str());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "[Error]: " << e.what() << endl;
    }
    // close sockets 
    if (udp_sock != -1) 
        close(udp_sock);
    
    if (tcp_sock != -1) 
        close(tcp_sock);
    
    exit(signum); 
}

bool verify_port(const string& port_str) {

    if (port_str.empty()) {
        return false;
    }

    for (char c : port_str) {
        if (!isdigit(c)) {
            return false;
        }
    }

    try {
        int port = stoi(port_str);
        return port >= 1 && port <= 65535;
    } catch (const exception&) {
        return false;
    }
}

void verify_args(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:v")) != -1) {
        switch (opt) {
            case 'p':
                server_port = optarg;
                if (!verify_port(server_port)) {
                    cerr << "Invalid port: " << server_port << endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                verbose = true;
                break;
            default:
                cerr << "Usage: " << argv[0] << " [-p GSport] [-v]" << endl;
                exit(EXIT_FAILURE);
        }
    }  
}

int create_udp_socket() {
    int fd;
    struct addrinfo hints{}, *res;

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) {
        cerr << "[Error]: create UDP socket failed." << endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // Socket UDP
    hints.ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(nullptr, server_port.c_str(), &hints, &res);
    if (errcode != 0) {
        cerr << "Address error: " << gai_strerror(errcode) << endl;
        exit(EXIT_FAILURE);
    }

    unique_ptr<addrinfo, decltype(&freeaddrinfo)> res_guard(res, freeaddrinfo);

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
        cerr << "[Error]: binding in UDP socket failed." << endl;
        exit(EXIT_FAILURE);
    }

    return fd;
}

int create_tcp_socket() {

    int fd;
    struct addrinfo hints{}, *res;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        cerr << "[Error]: create TCP socket failed." << endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // Socket TCP
    hints.ai_flags = AI_PASSIVE;

    /*if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        cerr << "[Error]: setsockopt(SO_REUSEADDR) failed." << endl;
    }*/

    int errcode = getaddrinfo(NULL, server_port.c_str(), &hints, &res);
    if (errcode != 0) {
        cerr << "Address error: " << gai_strerror(errcode) << endl;
        exit(EXIT_FAILURE);
    }

    ssize_t n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        cerr << "[Error]: binding in TCP socket failed." << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 128) == -1) {
        cerr << "[Error]: listen in TCP socket failed." << endl;
        exit(EXIT_FAILURE);
    }

    return fd;
}


int main(int argc, char** argv) {
    verify_args(argc, argv);

    setup_signal_handler(SIGINT, handle_sigint); // handle Ctrl+C
    setup_signal_handler(SIGCHLD, SIG_IGN);      // prevent zombie processes
    setup_signal_handler(SIGPIPE, SIG_IGN);      // ignore when writing to a closed socket
  
    int udp_socket = create_udp_socket();
    int tcp_socket = create_tcp_socket();
   
    int maxfd = max(udp_socket, tcp_socket);
    fd_set current_fds;

    while(true) {
        
        FD_ZERO(&current_fds);
        FD_SET(udp_socket, &current_fds);
        FD_SET(tcp_socket, &current_fds);
        //fd_set ready_fds = current_fds;

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (select(maxfd + 1, &current_fds, nullptr, nullptr, &timeout) == -1) {
            cerr << "[Error]: select failed." << endl;
            exit(EXIT_FAILURE);
        }

        // TCP socket ready
        if (FD_ISSET(tcp_socket, &current_fds)) {
            
            struct sockaddr_in player_addr;
            socklen_t player_addr_len = sizeof(player_addr);

            int player_sock = accept(tcp_socket, (struct sockaddr*)&player_addr, &player_addr_len);
            while (player_sock == -1 && errno == EINTR) 
                player_sock = accept(tcp_socket, (struct sockaddr *)&player_addr, (socklen_t *)&player_addr_len);
            
            if (player_sock == -1) {
                cerr << "[Error]: accept TCP player socket failed." << endl;
                exit(EXIT_FAILURE);
            }
            
            pid_t pid = fork();
            if (pid == -1) {
                cerr << "[Error]: forking failed." << endl;
                exit(EXIT_FAILURE);
            } 
            // CHILD PROCESS handles new connection
            else if (pid == 0) { 
                     
                if (close(tcp_socket) == -1) 
                    cerr << "[Error]: close TCP socket failed." << endl;
                
                read_TCP_socket(player_sock, player_addr, verbose);

                if (close(player_sock) == -1) 
                    cerr << "[Error]: close player TCP socket failed." << endl;
                
                exit(0);
            }

            // PARENT PROCESS closes the player_sock
            int ret = close(player_sock);
            while (ret == -1 && errno == EINTR) 
                ret = close(player_sock);
            
            if (ret == -1) {
                cerr << "[Error]: close TCP socket failed." << endl;
                exit(1);
            }
        }
        
        // UDP socket ready
        if (FD_ISSET(udp_socket, &current_fds)) {
            read_UDP_socket(udp_socket, verbose);
        }
    }   

    // close sockets
    if (close(udp_socket) < 0) 
        cerr << "[Error]: close UDP socket failed." << endl;
    
    if (close(tcp_socket) < 0) 
        cerr << "[Error]: close TCP socket failed." << endl;
    
    return 0;
}
