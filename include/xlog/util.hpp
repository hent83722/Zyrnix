#pragma once
#include <string>

namespace xlog {

inline std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

inline std::string to_upper(const std::string& s) {
    std::string result = s;
    for (auto& c : result) c = static_cast<char>(::toupper(c));
    return result;
}

inline std::string to_lower(const std::string& s) {
    std::string result = s;
    for (auto& c : result) c = static_cast<char>(::tolower(c));
    return result;
}

}
