#pragma once
#include "log_sink.hpp"
#include <iostream>

namespace xlog {

class StdoutSink : public LogSink {
public:
    void log(const std::string& logger_name, LogLevel level, const std::string& message) override;
};

}
