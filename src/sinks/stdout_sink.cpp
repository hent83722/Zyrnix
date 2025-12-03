#include "stdout_sink.hpp"

namespace xlog {

void StdoutSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    if (level < get_level()) return;
    std::cout << formatter.format(logger_name, level, message) << std::endl;
}

}
