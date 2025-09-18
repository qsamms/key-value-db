#pragma once

#include <exception>
#include <string>

class InvalidCommandException : public std::exception {
private:
    std::string message;

public:

    InvalidCommandException(const std::string& s) : message(s) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

    const int get_message_size() {
        return message.size();
    }

};