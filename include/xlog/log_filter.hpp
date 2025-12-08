#pragma once
#include <string>
#include <functional>
#include <memory>
#include "log_level.hpp"
#include "log_record.hpp"

namespace xlog {

class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool should_log(const LogRecord& record) const = 0;
};

class LevelFilter : public LogFilter {
public:
    explicit LevelFilter(LogLevel min_level);
    bool should_log(const LogRecord& record) const override;

private:
    LogLevel min_level_;
};

class FieldFilter : public LogFilter {
public:
    FieldFilter(const std::string& field_name, const std::string& expected_value);
    bool should_log(const LogRecord& record) const override;

private:
    std::string field_name_;
    std::string expected_value_;
};

class LambdaFilter : public LogFilter {
public:
    using FilterFunc = std::function<bool(const LogRecord&)>;
    explicit LambdaFilter(FilterFunc func);
    bool should_log(const LogRecord& record) const override;

private:
    FilterFunc filter_func_;
};

class CompositeFilter : public LogFilter {
public:
    enum class Mode { AND, OR };
    
    CompositeFilter(Mode mode);
    void add_filter(std::shared_ptr<LogFilter> filter);
    bool should_log(const LogRecord& record) const override;

private:
    Mode mode_;
    std::vector<std::shared_ptr<LogFilter>> filters_;
};

} // namespace xlog
