#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "utils/types.h"

extern std::map<std::string, db_entry> db;
extern std::mutex global_mutex;

int set(const Command& cmd);
int del(const std::string& key);
std::optional<db_entry> get(const std::string& key);
