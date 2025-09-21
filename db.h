#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "types.h"

extern std::map<std::string, db_entry> db;
extern std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
extern std::mutex global_mutex;

int set_value(const std::string& key, const Command& value);
std::optional<db_entry> get_value(const std::string& key);
std::string val_to_string(const db_value& value);