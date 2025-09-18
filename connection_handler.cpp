#include "connection_handler.h"
#include "exceptions.h"

#include <arpa/inet.h>
#include <algorithm>
#include <cctype>

std::string to_lower(const std::string& str) {
    std::string out(str);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
    return out;
}


Action string_to_action(const std::string& s) {
    std::string lower = to_lower(s);
    if (lower == "set") return SET;
    else if (lower == "setx") return SETEX;
    else if (lower == "get") return GET;
    else throw InvalidCommandException("Unknown action\n");
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

Command parse_command(const std::string& command_str) {
    std::vector<std::string> command_parts = split(command_str, ' ');

    if (command_parts.size() < 2) {
        throw InvalidCommandException("Invalid Command, all commands must have an action and key. Example: GET <key>\n");
    }
    
    Command c;
    c.action = string_to_action(command_parts[0]);
    c.key = command_parts[1];

    if (c.action == SET || c.action == SETEX) {
        if (command_parts.size() < 3) {
            throw InvalidCommandException("Inavlid Command, SET must be of form SET <key> <value>\n");
        }
        c.value = command_parts[2];
    } 
    return c;
}

void handle_connection(const uint32_t client_fd) {
    char buffer[1024];
    while (1) {
        try {
            int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_read > 0) {
                Request req;
                req.client_fd = client_fd;
                req.command_str = std::string(buffer);

                Command command = parse_command(req.command_str);

                std::cout << command.action << " " << command.value << std::endl;
            } else {
                // recv is blocking call until either data is received or the client
                // has terminated the connection, if bytes read is 0 conneciton was terminated. 
                break;
            }
        } catch (InvalidCommandException& e) {
            send(client_fd, e.what(), e.get_message_size(), 0);
        }
    }
    close(client_fd);
}
