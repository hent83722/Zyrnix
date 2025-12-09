#pragma once
#include "xlog_features.hpp"
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include "log_sink.hpp"
#include "log_level.hpp"
#include "log_record.hpp"

namespace xlog {

#ifndef XLOG_NO_FILTERS
class LogFilter;
#endif

class Logger {
public:
    explicit Logger(std::string name);

    void add_sink(LogSinkPtr sink);
    void clear_sinks();
    void log(LogLevel level, const std::string& message);

    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);
    
    void set_level(LogLevel level);
    LogLevel get_level() const;
    
#ifndef XLOG_NO_FILTERS
    void add_filter(std::shared_ptr<LogFilter> filter);
    void clear_filters();
    void set_filter_func(std::function<bool(const LogRecord&)> func);
#endif
    
    static std::shared_ptr<Logger> create_stdout_logger(const std::string& name);
    
#ifndef XLOG_NO_ASYNC
    static std::shared_ptr<Logger> create_async(const std::string& name);
#endif
    
    std::string name;

private:
    bool should_log(const LogRecord& record) const;
    
    std::vector<LogSinkPtr> sinks;
#ifndef XLOG_NO_FILTERS
    std::vector<std::shared_ptr<LogFilter>> filters_;
    std::function<bool(const LogRecord&)> filter_func_;
#endif
    LogLevel min_level_;
    std::mutex mtx;
};

using LoggerPtr = std::shared_ptr<Logger>;

}
