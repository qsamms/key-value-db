#include "db.h"

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

std::map<std::string, db_value> db;
std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
std::mutex global_mutex;

int set_value(const std::string& key, const db_value& value) {
    try {
        std::unique_lock<std::mutex> g(global_mutex);
        if (!db.contains(key)) {
            key_mutexes[key] = std::make_unique<std::mutex>();
        }
        g.unlock();
        std::lock_guard<std::mutex> lk(*key_mutexes[key]);
        db[key] = value;
    } catch (std::exception& e) {
        std::cout << "Error setting key: " << key << std::endl;
        return 0;
    }
    return 1;
}

std::optional<db_value> get_value(const std::string& key) {
    std::unique_lock<std::mutex> g(global_mutex);
    if (!db.contains(key)) {
        return std::nullopt;
    }
    g.unlock();
    std::lock_guard<std::mutex> lk(*key_mutexes[key]);
    return db[key];
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
