#pragma once

#include <string>
#include <cstdint>

struct db_entry {
    std::string value;
    int64_t expiration;
};

enum Action {
    ACTION_SET,
    ACTION_SETEX,
    ACTION_GET,
    ACTION_DELETE,
    ACTION_PERSIST,
    ACTION_EXPIRE,
};

struct Command {
    Action action;
    std::string key;
    std::string value;
    int64_t expiration;
};