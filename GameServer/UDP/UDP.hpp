#ifndef UDP_HPP
#define UDP_HPP
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>	
#include <netdb.h>
#include "start/start.hpp"
#include "try/try.hpp"
#include "quit_exit/quit_exit.hpp"
#include "debug/debug.hpp"
#define START_CMD "SNG"
#define TRY_CMD "TRY"
#define QUIT_EXIT_CMD "QUT"
#define DEBUG_CMD "DBG"
#define MODE_PLAY "P"
#define MODE_DEBUG "D"

using namespace std;
extern vector<char> colors;

vector<string> split_string(const string& str, char delimiter);

void read_UDP_socket(int fd, bool verbose);

void process_UDP_request(const string& request, int fd, struct sockaddr_in addr,
                         socklen_t addrlen, bool verbose);

void check_verbose(const string& cmd, const string& plid,
                 struct sockaddr_in addr, bool verbose, string& response);

void check_verbose(const string& cmd, struct sockaddr_in addr, bool verbose, string& response);

#endif