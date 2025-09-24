#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "utils/types.h"

extern std::map<std::string, db_entry> db;
extern std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
extern std::mutex global_mutex;

int set(const std::string& key, const Command& value);
int del(const std::string& key);
std::optional<db_entry> get(const std::string& key);
std::string val_to_string(const db_value& value);