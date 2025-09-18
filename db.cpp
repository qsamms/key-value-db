#include "db.h"

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

std::map<std::string, db_value> db;
std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
std::mutex global_mutex;

int set_value(std::string &key, db_value &value) {
  try {
    std::unique_lock<std::mutex> g(global_mutex);
    if (!db.contains(key)) {
      key_mutexes[key] = std::make_unique<std::mutex>();
    }
    g.unlock();
    std::lock_guard<std::mutex> lk(*key_mutexes[key]);
    db[key] = value;
  } catch (std::exception &e) {
    std::cout << "Error setting key: " << key << std::endl;
    return -1;
  }
  return 1;
}

db_value get_value(std::string &key) {
  std::unique_lock<std::mutex> g(global_mutex);
  if (!db.contains(key)) {
    return nullptr;
  }
  g.unlock();
  std::lock_guard<std::mutex> lk(*key_mutexes[key]);
  return db[key];
}