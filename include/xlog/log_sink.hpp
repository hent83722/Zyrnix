#pragma once
#include "formatter.hpp"
#include "log_level.hpp"
#include <string>
#include <memory>

namespace xlog {

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void log(const std::string& logger_name, LogLevel level, const std::string& message) = 0;
    void set_level(LogLevel lvl) { min_level = lvl; }
    LogLevel get_level() const { return min_level; }

protected:
    LogLevel min_level = LogLevel::Trace;
    Formatter formatter;
};

using LogSinkPtr = std::shared_ptr<LogSink>;

}
