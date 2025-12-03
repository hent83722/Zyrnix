#pragma once
#include "log_level.hpp"
#include <string>

namespace xlog {

inline std::string color_code(LogLevel level) {
    switch(level) {
        case LogLevel::Trace: return "\033[37m";
        case LogLevel::Debug: return "\033[36m";
        case LogLevel::Info: return "\033[32m";
        case LogLevel::Warn: return "\033[33m";
        case LogLevel::Error: return "\033[31m";
        case LogLevel::Critical: return "\033[41m";
        default: return "\033[0m";
    }
}

inline std::string reset_code() { return "\033[0m"; }

}
