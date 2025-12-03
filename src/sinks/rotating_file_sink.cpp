#include "rotating_file_sink.hpp"
#include <filesystem>

namespace xlog {

RotatingFileSink::RotatingFileSink(const std::string& base, size_t max_s, size_t max_f)
    : base_name(base), max_size(max_s), max_files(max_f) {
    open_file();
}

void RotatingFileSink::open_file() {
    file.open(base_name + ".log", std::ios::app);
    if (file.is_open()) current_size = std::filesystem::file_size(base_name + ".log");
}

void RotatingFileSink::rotate() {
    file.close();
    for (size_t i = max_files; i > 0; --i) {
        std::string old_name = base_name + "." + std::to_string(i-1) + ".log";
        std::string new_name = base_name + "." + std::to_string(i) + ".log";
        if (std::filesystem::exists(old_name)) std::filesystem::rename(old_name, new_name);
    }
    std::filesystem::rename(base_name + ".log", base_name + ".0.log");
    open_file();
}

void RotatingFileSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    if (level < get_level()) return;
    std::lock_guard<std::mutex> lock(mtx);
    std::string line = formatter.format(logger_name, level, message) + "\n";
    file << line;
    current_size += line.size();
    if (current_size >= max_size) rotate();
}

}
