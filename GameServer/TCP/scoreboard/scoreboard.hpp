#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP
#include <string>
#include <vector>
#include "../../constants.hpp"
using namespace std;

struct SCORELIST {
    vector<string> score;
    vector<string> plid;
    vector<string> secret_key;
    vector<string> nr_tries;
    vector<string> mode;
    int nr_scores;
};

int find_top10_scores(SCORELIST& scorelist);

int process_scoreboard(string& response, int fd, const string& cmd,
                    struct sockaddr_in addr, bool verbose);

string create_SB_file(SCORELIST& scorelist);

#endif