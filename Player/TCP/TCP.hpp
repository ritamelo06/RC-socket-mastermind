#ifndef TCP_HPP
#define TCP_HPP
#include <string>
#include <vector>
#include "scoreboard/scoreboard.hpp"
#include "show_trials/show_trials.hpp"
#include "../player.hpp"
using namespace std;

bool is_tcp_command(const string& command);

void handle_tcp_cmd(const string& command, const string& input, Player* player, const string& server_ip, const string& server_port);

int send_tcp_request(const string& request, const string& server_ip, const string& server_port, Player* player);

void end_player_app (Player* player, int fd, struct addrinfo* res);

void display_TCP_response(int fd, Player* player, struct addrinfo* res);

string read_word_from_socket(int socket_fd);

int create_local_copy(const string& fname, const string& fsize, const string& dir, bool is_show_trials);

#endif