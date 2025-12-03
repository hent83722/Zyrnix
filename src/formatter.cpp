#include "xlog/formatter.hpp"
#include "xlog/log_sink.hpp"
#include "xlog/log_level.hpp"
#include "xlog/color.hpp"

namespace xlog {

std::string Formatter::format(const std::string& logger_name, LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_r(&t_c, &tm_buf);
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    ss << " [" << to_string(level) << "] ";
    ss << logger_name << ": " << message;
    return ss.str();
}

}
