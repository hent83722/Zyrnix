#pragma once
#include "xlog/log_sink.hpp"
#include <memory>
#include <string>

namespace xlog {

class StdoutSink : public LogSink {
public:
    StdoutSink();
    void log(const std::string& name, LogLevel level, const std::string& message) override;
};

}
