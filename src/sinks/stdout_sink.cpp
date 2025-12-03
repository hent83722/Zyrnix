#include "xlog/sinks/stdout_sink.hpp"
#include "xlog/log_sink.hpp"
#include <iostream>
#include <mutex>

namespace xlog {

void StdoutSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    if (level < get_level()) return;
    std::cout << formatter.format(logger_name, level, message) << std::endl;
}

}
