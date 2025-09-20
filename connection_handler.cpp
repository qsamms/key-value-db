#include "connection_handler.h"

#include <arpa/inet.h>

#include <regex>

#include "db.h"
#include "exceptions.h"
#include "response_codes.h"
#include "utils.h"

std::regex int_re(R"(^[+-]?\d+$)");
std::regex float_re(R"(^[+-]?\d*\.\d+([eE][+-]?\d+)?$)");
std::regex sci_re(R"(^[+-]?\d+([eE][+-]?\d+)$)");

enum Action {
    ACTION_SET,
    ACTION_SETEX,
    ACTION_GET,
};

struct Command {
    Action action;
    std::string key;
    db_value value;
    std::time_t expiration;
};

db_value value_from_string(const std::string& value) {
    if (std::regex_match(value, int_re))
        return std::stoi(value.c_str());
    else if (std::regex_match(value, float_re) || std::regex_match(value, sci_re))
        return std::stod(value.c_str());
    else
        return value;
}

Action string_to_action(const std::string& s) {
    std::string lower = to_lower(s);
    if (lower == "set")
        return ACTION_SET;
    else if (lower == "setx")
        return ACTION_SETEX;
    else if (lower == "get")
        return ACTION_GET;
    else
        throw InvalidCommandException();
}

Command parse_command(const std::string& command_str) {
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
    else if (command.action == ACTION_SET || command.action == ACTION_SETEX) {
        if (command_value.size() == 0) throw InvalidCommandException();
        command.value = value_from_string(command_value);
    }
    return command;
}

std::string perform_command(Command& command) {
    std::string key = command.key;
    Action action = command.action;

    if (action == ACTION_GET) {
        std::optional<db_value> value = get_value(key);
        if (!value) return ERR_NOT_FOUND;
        return val_to_string(value.value());
    } else if (action == ACTION_SET) {
        int status = set_value(key, command.value);
        if (status) {
            return OK;
        } else
            return ERR_SETTING_VALUE;
    }
}

void handle_connection(const uint32_t client_fd) {
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
    close(client_fd);
}
