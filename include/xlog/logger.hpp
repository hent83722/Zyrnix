#pragma once
#include "log_sink.hpp"
#include "log_level.hpp"
#include <vector>
#include <string>
#include <memory>
#include <mutex>

namespace xlog {

class Logger {
public:
    explicit Logger(std::string name);
    void add_sink(LogSinkPtr sink);
    void log(LogLevel level, const std::string& message);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

private:
    std::string name;
    std::vector<LogSinkPtr> sinks;
    std::mutex mtx;
};

using LoggerPtr = std::shared_ptr<Logger>;

}
