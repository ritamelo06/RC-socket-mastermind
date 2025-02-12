#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#define MAX_PLAYTIME 600
#include <string>

constexpr char DEFAULT_IP[] = "localhost";
constexpr char DEFAULT_PORT[] = "58025";  // 58000 + group number
constexpr int MAX_BUFFER_SIZE = 128;
constexpr int MAX_COMMAND_SIZE = 128;
constexpr int SIZE_PLID = 6;
constexpr int MAX_TCP_REQ = 11;  // "STR XXXXXX\n" 


#endif 