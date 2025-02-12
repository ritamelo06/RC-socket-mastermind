#ifndef TCP_HPP
#define TCP_HPP
#include <string>
#include <vector>
#define SHOWTRIALS_CMD "STR"
#define SCOREBOARD_CMD "SSB"
#include "show_trials/show_trials.hpp"
#include "scoreboard/scoreboard.hpp"

void read_TCP_socket(int fd, struct sockaddr_in addr, bool verbose);

void process_TCP_request(const string& request, int fd, struct sockaddr_in addr , bool verbose);

void send_tcp_file(int fd, string response, string file_path, string plid,
                    string cmd, struct sockaddr_in addr, bool verbose);

string read_word_TCP_socket(int socket_fd, int size);

#endif // TCP_HPP