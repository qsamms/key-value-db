#pragma once

#include <string>
#include <variant>

using db_value = std::variant<int, long, long long, double, std::string>;

struct db_entry {
    db_value value;
    int64_t expiration;
};

enum Action {
    ACTION_SET,
    ACTION_SETEX,
    ACTION_GET,
};

struct Command {
    Action action;
    std::string key;
    db_value value;
    uint64_t expiration;
};