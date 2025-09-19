#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <variant>

using db_value = std::variant<int, long, long long, double, std::string>;

extern std::map<std::string, db_value> db;
extern std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
extern std::mutex global_mutex;

int set_value(const std::string& key, const db_value& value);
std::optional<db_value> get_value(const std::string& key);
std::string val_to_string(const db_value& value);