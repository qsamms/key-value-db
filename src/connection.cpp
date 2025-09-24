#include "connection.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <regex>

#include "db.h"
#include "utils/exceptions.h"
#include "utils/response_codes.h"
#include "utils/types.h"
#include "utils/utils.h"

const std::regex Connection::int_re(R"(^[+-]?\d+$)");
const std::regex Connection::float_re(R"(^[+-]?\d*\.\d+([eE][+-]?\d+)?$)");
const std::regex Connection::sci_re(R"(^[+-]?\d+([eE][+-]?\d+)$)");

db_value Connection::value_from_string(const std::string& value) {
    if (value.size() == 0) throw InvalidCommandException();

    if (std::regex_match(value, int_re))
        return std::stoi(value.c_str());
    else if (std::regex_match(value, float_re) || std::regex_match(value, sci_re))
        return std::stod(value.c_str());
    else
        return value;
}

Action Connection::string_to_action(const std::string& s) {
    std::string lower = to_lower(s);
    if (lower == "set")
        return ACTION_SET;
    else if (lower == "setex")
        return ACTION_SETEX;
    else if (lower == "get")
        return ACTION_GET;
    else
        throw InvalidCommandException();
}

Command Connection::parse_command(const std::string& command_str) {
    std::vector<std::string> command_parts = split(command_str, ' ');

    if (command_parts.size() < 2) throw InvalidCommandException();

    std::string command_action = command_parts[0];
    std::string command_key = command_parts[1];
    std::string command_value;

    if (command_parts.size() > 2) command_value = command_parts[2];

    Command command;
    command.action = string_to_action(command_action);
    command.key = command_key;

    if (command.action == ACTION_GET)
        command.value = 0;
    else if (command.action == ACTION_SET) {
        command.value = value_from_string(command_value);
        command.expiration = -1;
    } else if (command.action == ACTION_SETEX) {
        if (command_parts.size() < 4) throw InvalidCommandException();

        command.value = value_from_string(command_value);

        std::string expiration_str = command_parts[3];
        if (!std::regex_match(expiration_str, int_re)) throw InvalidCommandException();
        int expiration_seconds = std::stoi(expiration_str);
        if (expiration_seconds < 0) throw InvalidCommandException();

        auto now = std::chrono::system_clock::now();
        auto seconds_since_epoch =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        command.expiration = (int64_t)seconds_since_epoch + expiration_seconds;
    }
    return command;
}

std::string Connection::perform_command(Command& command) {
    std::string key = command.key;
    Action action = command.action;

    if (action == ACTION_GET) {
        std::optional<db_entry> entry = get_value(key);
        if (!entry) return ERR_NOT_FOUND;
        return val_to_string(entry.value().value);
    } else if (action == ACTION_SET || action == ACTION_SETEX) {
        int status = set_value(key, command);
        if (status) {
            return OK;
        } else
            return ERR_SETTING_VALUE;
    }
}

void Connection::handle_connection(const uint32_t client_fd) {
    char buffer[1024];
    while (1) {
        try {
            int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_read <= 0) break;  // exit if client terminates connection

            uint16_t command_str_size = bytes_read;
            if (buffer[bytes_read - 1] == '\n') command_str_size--;

            Command command = parse_command(std::string(buffer, command_str_size));
            std::string response = perform_command(command);
            send(client_fd, response.c_str(), response.size(), 0);

        } catch (InvalidCommandException& e) {
            send(client_fd, e.what(), e.get_message_size(), 0);
        } catch (std::exception& e) {
            send(client_fd, ERR_UNKNOWN.c_str(), ERR_UNKNOWN.size(), 0);
        }
    }
}

Connection::Connection(int client_fd) {
    client_fd_ = client_fd;
    handle_connection(client_fd);
}

Connection::~Connection() {
    close(client_fd_);
}