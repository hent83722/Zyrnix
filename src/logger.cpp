#include "xlog/logger.hpp"
#include "xlog/log_sink.hpp"
#include "xlog/log_filter.hpp"
#include "xlog/sinks/stdout_sink.hpp"
#include "xlog/async/async_logger.hpp"
#include <mutex>
#include <chrono>

namespace xlog {

Logger::Logger(std::string n) 
    : name(std::move(n)), min_level_(LogLevel::Trace) {}

void Logger::add_sink(LogSinkPtr sink) {
    std::lock_guard<std::mutex> lock(mtx);
    sinks.push_back(std::move(sink));
}

void Logger::clear_sinks() {
    std::lock_guard<std::mutex> lock(mtx);
    sinks.clear();
}

void Logger::set_level(LogLevel level) {
    min_level_ = level;
}

LogLevel Logger::get_level() const {
    return min_level_;
}

void Logger::add_filter(std::shared_ptr<LogFilter> filter) {
    std::lock_guard<std::mutex> lock(mtx);
    filters_.push_back(std::move(filter));
}

void Logger::clear_filters() {
    std::lock_guard<std::mutex> lock(mtx);
    filters_.clear();
    filter_func_ = nullptr;
}

void Logger::set_filter_func(std::function<bool(const LogRecord&)> func) {
    std::lock_guard<std::mutex> lock(mtx);
    filter_func_ = std::move(func);
}

bool Logger::should_log(const LogRecord& record) const {
    if (record.level < min_level_) {
        return false;
    }
    
    if (filter_func_ && !filter_func_(record)) {
        return false;
    }
    
    for (const auto& filter : filters_) {
        if (!filter->should_log(record)) {
            return false;
        }
    }
    
    return true;
}

void Logger::log(LogLevel level, const std::string& message) {
    LogRecord record;
    record.logger_name = name;
    record.level = level;
    record.message = message;
    record.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(mtx);
    
    if (!should_log(record)) {
        return;
    }
    
    for (auto& s : sinks) {
        s->log(name, level, message);
    }
}

void Logger::trace(const std::string& msg) { log(LogLevel::Trace, msg); }
void Logger::debug(const std::string& msg) { log(LogLevel::Debug, msg); }
void Logger::info(const std::string& msg) { log(LogLevel::Info, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::Warn, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::Error, msg); }
void Logger::critical(const std::string& msg) { log(LogLevel::Critical, msg); }

std::shared_ptr<Logger> Logger::create_stdout_logger(const std::string& name) {
    auto logger = std::make_shared<Logger>(name);
    logger->add_sink(std::make_shared<StdoutSink>());
    return logger;
}

std::shared_ptr<Logger> Logger::create_async(const std::string& name) {
    auto logger = std::make_shared<Logger>(name);
    return logger;
}

}
