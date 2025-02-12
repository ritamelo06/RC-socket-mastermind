#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <string>

constexpr char DEFAULT_PORT[] = "58025";  // 58000 + group number
constexpr int MAX_BUFFER_SIZE = 512;
constexpr int MAX_SIZE = 128;
constexpr int MAX_PLAYTIME = 600;
constexpr int MAX_TCP = 3000; 
constexpr int MAX_TCP_REQUEST = 11;
constexpr int SIZE_TCP_CMD = 3;
constexpr int SIZE_PLID = 6;
constexpr int MAX_TCP_RESPONSE = 8; // "RST XXX\n" ou "RSS XXX\n"

#endif // CONSTANTS_H
