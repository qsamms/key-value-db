#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <utils/types.h>

extern std::map<std::string, db_entry> db;
extern std::mutex global_mutex;

std::optional<db_entry> get(const std::string& key);
int set(const Command& cmd);
int del(const std::string& key);
int persist(const Command& cmd);
int expire(const Command& cmd);
