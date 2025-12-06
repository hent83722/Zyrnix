#pragma once
#include "xlog/log_sink.hpp"
#include <string>
#include <memory>

namespace xlog {

class NullSink : public LogSink {
public:
    void log(const std::string& logger_name, LogLevel level, const std::string& message) override {}
};

}
