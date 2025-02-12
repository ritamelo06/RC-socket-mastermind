#ifndef UDP_HPP
#define UDP_HPP
#include <string>
#include <vector>
#include "start/start.hpp"
#include "try/try.hpp"
#include "quit_exit/quit_exit.hpp"
#include "debug/debug.hpp"
#include "../player.hpp"
using namespace std;
#define MAX_RES_SIZE 128

bool is_udp_command(const string& command);

vector<string> split_string(const string& str, char delimiter);

int send_udp_request(const string& request, const string& server_ip, const string& server_port, Player* player, bool is_exit);

void handle_udp_cmd(const string& command, const string& input, Player* player, const string& server_ip, const string& server_port);

void display_udp_response(string& udp_reponse, Player* player, bool is_exit);

#endif