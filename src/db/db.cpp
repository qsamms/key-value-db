#include "db.h"

#include <utils/types.h>
#include <utils/utils.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

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

    auto it = db.find(key);
    if (it == db.end()) {
        return std::nullopt;
    }
    db_entry entry = it->second;
    int64_t expiration = entry.expiration;

    if (expiration > 0) {
        if (seconds_since_epoch() > expiration) {
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

int persist(const Command& cmd) {
    std::lock_guard<std::mutex> g(global_mutex);
    auto it = db.find(cmd.key);
    if (!(it == db.end())) {
        db_entry entry = it->second;
        if (entry.expiration > 0 && seconds_since_epoch() > entry.expiration) {
            db.erase(cmd.key);
            return 0;
        }
        entry.expiration = -1;
        return 1;
    }
    return 0;
}

int expire(const Command& cmd) {
    std::lock_guard<std::mutex> g(global_mutex);
    auto it = db.find(cmd.key);
    if (!(it == db.end())) {
        it->second.expiration = cmd.expiration;
        return 1;
    }
    return 0;
}