#include <map>
#include <string>
#include <mutex>
#include <variant>
#include <memory>

using db_value = std::variant<int, double, std::string>;

extern std::map<std::string, db_value> db;
extern std::map<std::string, std::unique_ptr<std::mutex>> key_mutexes;
extern std::mutex global_mutex;

int set_value(db_value &value);
db_value get_value(std::string &key);