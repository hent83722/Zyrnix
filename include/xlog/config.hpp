#pragma once
#include "log_level.hpp"
#include <string>

namespace xlog {

struct Config {
    LogLevel default_level = LogLevel::Trace;
    std::string default_pattern = "%Y-%m-%d %H:%M:%S [%l] %n: %v";
    size_t async_queue_size = 8192;
};

}
