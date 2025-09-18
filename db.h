#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <variant>

using db_value = std::variant<int, long, long long, double, std::string>;

inline std::ostream &operator<<(std::ostream &os, const db_value &v) {
  std::visit([&os](auto &&arg) { os << arg; }, v);
  return os;
}

extern std::map<std::string, db_value> db;
extern std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
extern std::mutex global_mutex;

int set_value(db_value &value);
db_value get_value(std::string &key);