#include "xlog/sinks/stdout_sink.hpp"
#include <iostream>

namespace xlog {

StdoutSink::StdoutSink() {}

void StdoutSink::log(const std::string& name, LogLevel level, const std::string& message) {
    std::cout << "[" << static_cast<int>(level) << "] "
              << name << ": "
              << message << std::endl;
}

}
