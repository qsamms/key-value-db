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

Action Connection::string_to_action(const std::string& s) {
    std::string lower = to_lower(s);
    if (lower == "set")
        return ACTION_SET;
    else if (lower == "setex")
        return ACTION_SETEX;
    else if (lower == "get")
        return ACTION_GET;
    else if (lower == "del")
        return ACTION_DELETE;
    else if (lower == "persist")
        return ACTION_PERSIST;
    else if (lower == "expire")
        return ACTION_EXPIRE;
    else
        throw InvalidCommandException("Unknown action");
}

void Connection::parse_and_set_expiration(Command& cmd, const std::string& expiration_str) {
    if (!std::regex_match(expiration_str, int_re))
        throw InvalidCommandException("expiration must be an integer");

    int expiration_seconds = std::stoi(expiration_str);
    if (expiration_seconds < 0) throw InvalidCommandException("expiration must be > 0");

    auto now = std::chrono::system_clock::now();
    auto seconds_since_epoch =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    cmd.expiration = (int64_t)seconds_since_epoch + expiration_seconds;
}

Command Connection::parse_command(const std::string& command_str) {
    std::vector<std::string> command_parts = split(command_str, ' ');
    if (command_parts.size() < 2)
        throw InvalidCommandException("All commands require an action and key");

    Command cmd;
    cmd.action = string_to_action(command_parts[0]);
    cmd.key = command_parts[1];
    cmd.value = "";
    cmd.expiration = -1;

    if (cmd.action == ACTION_SET) {
        if (command_parts.size() != 3) throw InvalidCommandException("'set' must have 3 operands");
        cmd.value = command_parts[2];
    }

    else if (cmd.action == ACTION_SETEX) {
        if (command_parts.size() != 4)
            throw InvalidCommandException("'setex' must have 4 operands");
        cmd.value = command_parts[2];
        parse_and_set_expiration(cmd, command_parts[3]);
    }

    else if (cmd.action == ACTION_EXPIRE) {
        if (command_parts.size() != 3)
            throw InvalidCommandException("'expire' must have 3 operands");
        parse_and_set_expiration(cmd, command_parts[2]);
    }

    return cmd;
}

std::string Connection::perform_command(Command& cmd) {
    std::string key = cmd.key;
    Action action = cmd.action;

    if (action == ACTION_GET) {
        std::optional<db_entry> entry = get(key);
        if (!entry) return ERR_NOT_FOUND;
        return entry.value().value + "\n";
    }

    else if (action == ACTION_SET || action == ACTION_SETEX) {
        if (set(cmd)) return OK;
        return ERR_UNKNOWN;
    }

    else if (action == ACTION_DELETE) {
        if (del(key)) return OK;
        return ERR_NOT_FOUND;
    }

    else if (action == ACTION_EXPIRE) {
        if (expire(cmd)) return OK;
        return ERR_NOT_FOUND;
    }

    else if (action == ACTION_PERSIST) {
        if (persist(cmd)) return OK;
        return ERR_NOT_FOUND;
    }

    else
        throw InvalidCommandException("unknown action");
}

void Connection::handle_connection(const uint32_t client_fd) {
    char buffer[1024];
    while (1) {
        try {
            int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_read <= 0) break;  // exit if client terminates connection

            uint16_t cmd_str_size = bytes_read;
            if (buffer[bytes_read - 1] == '\n') cmd_str_size--;

            Command cmd = parse_command(std::string(buffer, cmd_str_size));
            std::string response = perform_command(cmd);
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