#include "db.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "utils/types.h"

std::map<std::string, db_entry> db;
std::mutex global_mutex;

int set(const Command& cmd) {
    std::lock_guard<std::mutex> g(global_mutex);
    db_entry entry;
    entry.expiration = cmd.expiration;
    entry.value = cmd.value;
    db[cmd.key] = entry;
    return 1;
}

std::optional<db_entry> get(const std::string& key) {
    std::lock_guard<std::mutex> g(global_mutex);
    if (!db.contains(key)) {
        return std::nullopt;
    }

    auto it = db.find(key);
    if (it == db.end()) {
        return std::nullopt;
    }
    db_entry entry = it->second;
    int64_t expiration = entry.expiration;

    if (expiration > 0) {
        auto now = std::chrono::system_clock::now();
        auto seconds_since_epoch =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        if (seconds_since_epoch > expiration) {
            db.erase(key);
            return std::nullopt;
        }
    }
    return entry;
}

int del(const std::string& key) {
    std::lock_guard<std::mutex> g(global_mutex);
    if (db.contains(key)) {
        db.erase(key);
        return 1;
    }
    return 0;
}
