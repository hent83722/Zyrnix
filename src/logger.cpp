#include "logger.hpp"

namespace xlog {

Logger::Logger(std::string logger_name) : name(std::move(logger_name)) {}

void Logger::add_sink(LogSinkPtr sink) {
    std::lock_guard<std::mutex> lock(mtx);
    sinks.push_back(std::move(sink));
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& sink : sinks) {
        sink->log(name, level, message);
    }
}

void Logger::trace(const std::string& message) { log(LogLevel::Trace, message); }
void Logger::debug(const std::string& message) { log(LogLevel::Debug, message); }
void Logger::info(const std::string& message) { log(LogLevel::Info, message); }
void Logger::warn(const std::string& message) { log(LogLevel::Warn, message); }
void Logger::error(const std::string& message) { log(LogLevel::Error, message); }
void Logger::critical(const std::string& message) { log(LogLevel::Critical, message); }

}
