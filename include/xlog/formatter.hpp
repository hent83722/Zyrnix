#pragma once
#include "log_level.hpp"
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace xlog {

class Formatter {
public:
    std::string format(const std::string& logger_name, LogLevel level, const std::string& message);
};

}
