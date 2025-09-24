#pragma once

#include "types.h"

#include <string>
#include <vector>

std::string to_lower(const std::string& str);
std::vector<std::string> split(const std::string& s, char delimiter);
std::string val_to_string(const db_value& value);
