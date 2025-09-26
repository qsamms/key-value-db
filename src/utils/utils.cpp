#include "utils.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <sstream>

std::string to_lower(const std::string& str) {
    std::string out(str);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
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

uint64_t seconds_since_epoch() {
    auto now = std::chrono::system_clock::now();
    return duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}