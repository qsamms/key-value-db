#pragma once

#include <ctime>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "db.h"

enum Action {
    ACTION_SET,
    ACTION_SETEX,
    ACTION_GET,
};

struct Command {
    Action action;
    std::string key;
    db_value value;
    std::time_t expiration;
};

std::vector<std::string> split(const std::string& s, char delimiter);
Action string_to_action(const std::string& s);
Command parse_command(const std::string& command);
std::string perform_command(Command& command);
void handle_connection(const uint32_t client_fd);
