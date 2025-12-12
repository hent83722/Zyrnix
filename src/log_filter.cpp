#include "xlog/log_filter.hpp"
#include "xlog/log_context.hpp"

namespace xlog {

LevelFilter::LevelFilter(LogLevel min_level) : min_level_(min_level) {}

bool LevelFilter::should_log(const LogRecord& record) const {
    return record.level >= min_level_;
}

FieldFilter::FieldFilter(const std::string& field_name, const std::string& expected_value)
    : field_name_(field_name), expected_value_(expected_value) {}

bool FieldFilter::should_log(const LogRecord& record) const {
    auto context = LogContext::get_all();
    auto it = context.find(field_name_);
    if (it != context.end()) {
        return it->second == expected_value_;
    }
    
    return record.get_field(field_name_) == expected_value_;
}

LambdaFilter::LambdaFilter(FilterFunc func) : filter_func_(std::move(func)) {}

bool LambdaFilter::should_log(const LogRecord& record) const {
    return filter_func_(record);
}

CompositeFilter::CompositeFilter(Mode mode) : mode_(mode) {}

void CompositeFilter::add_filter(std::shared_ptr<LogFilter> filter) {
    filters_.push_back(std::move(filter));
}

bool CompositeFilter::should_log(const LogRecord& record) const {
    if (filters_.empty()) {
        return true;
    }
    
    if (mode_ == Mode::AND) {
        for (const auto& filter : filters_) {
            if (!filter->should_log(record)) {
                return false;
            }
        }
        return true;
    } else {
        for (const auto& filter : filters_) {
            if (filter->should_log(record)) {
                return true;
            }
        }
        return false;
    }
}

RegexFilter::RegexFilter(const std::string& pattern, bool invert)
    : pattern_str_(pattern), regex_(pattern), invert_(invert) {}

RegexFilter::RegexFilter(const std::string& field_name, const std::string& pattern, bool invert)
    : pattern_str_(pattern), regex_(pattern), field_name_(field_name), invert_(invert) {}

bool RegexFilter::should_log(const LogRecord& record) const {
    std::string target;
    
    if (field_name_.empty()) {
        target = record.message;
    } else {
        auto context = LogContext::get_all();
        auto it = context.find(field_name_);
        if (it != context.end()) {
            target = it->second;
        } else {
            target = record.get_field(field_name_);
        }
    }
    
    bool matches = std::regex_search(target, regex_);
    return invert_ ? !matches : matches;
}

} 
