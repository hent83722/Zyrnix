#include "xlog/sinks/file_sink.hpp"
#include <iostream> 
#include <string>

namespace xlog {

FileSink::FileSink(const std::string& filename)
    : file(filename, std::ios::app) {
    if (!file.is_open()) {
        std::cerr << "Error opening log file: " << filename << std::endl;
    }
}

void FileSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!file.is_open()) return;

    file << "[" << static_cast<int>(level) << "] "
         << logger_name << ": "
         << message << std::endl;
}

}
