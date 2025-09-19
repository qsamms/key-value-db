#pragma once

#include "response_codes.h"

#include <exception>
#include <string>

class InvalidCommandException : public std::exception {
   private:
    std::string message;

   public:
    InvalidCommandException(const std::string& s) : message(s) {}
    InvalidCommandException() : message(ERR_INVALID_COMMAND) {}

    const char* what() const noexcept override { return message.c_str(); }

    const int get_message_size() { return message.size(); }
};