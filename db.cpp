#include "db.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "types.h"

std::map<std::string, db_entry> db;
std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
std::mutex global_mutex;

int set_value(const std::string& key, const Command& command) {
    try {
        db_entry entry;
        entry.expiration = command.expiration;
        entry.value = command.value;
        std::unique_lock<std::mutex> g(global_mutex);
        if (!db.contains(key)) {
            key_mutexes[key] = std::make_unique<std::mutex>();
        }
        std::lock_guard<std::mutex> lk(*key_mutexes[key]);
        g.unlock();
        db[key] = entry;
    } catch (std::exception& e) {
        std::cout << "Error setting key: " << key << std::endl;
        return 0;
    }
    return 1;
}

std::optional<db_entry> get_value(const std::string& key) {
    std::unique_lock<std::mutex> g(global_mutex);
    if (!db.contains(key)) {
        return std::nullopt;
    }
    std::lock_guard<std::mutex> lk(*key_mutexes[key]);
    g.unlock();

    db_entry entry = db[key];
    if (entry.expiration > 0) {
        auto now = std::chrono::system_clock::now();
        auto seconds_since_epoch =
            duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        if (seconds_since_epoch > entry.expiration) {
            db.erase(key);
            return std::nullopt;
        }
    }
    return entry;
}

std::string val_to_string(const db_value& v) {
    return std::visit(
        [](auto&& arg) -> std::string {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>) {
                return std::to_string(arg);
            } else {
                return arg;
            }
        },
        v);
}
