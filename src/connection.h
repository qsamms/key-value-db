#pragma once

#include <cstdint>
#include <regex>
#include <string>

#include "utils/types.h"

class Connection {
   private:
    int client_fd_;
    const static std::regex int_re;
    const static std::regex float_re;
    const static std::regex sci_re;

    db_value value_from_string(const std::string& value);
    Action string_to_action(const std::string& s);
    Command parse_command(const std::string& command_str);
    std::string perform_command(Command& command);

   public:
    void handle_connection(const uint32_t client_fd);

    Connection(int client_fd);
    ~Connection();
};