#pragma once
#include <string>
#include <memory>
#include "xlog/log_level.hpp"

namespace xlog {

struct LogMessage {
    LogLevel level;
    std::string text;
};

}
