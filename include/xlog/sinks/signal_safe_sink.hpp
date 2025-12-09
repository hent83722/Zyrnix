#pragma once

#include "../log_sink.hpp"
#include "../log_record.hpp"
#include <atomic>
#include <array>
#include <cstring>

namespace xlog {

/**
 * @brief Async-signal-safe logging sink for crash handlers
 * 
 * This sink is designed to be used in signal handlers and crash handlers
 * where only async-signal-safe functions are allowed. It uses a lock-free
 * ring buffer and only async-signal-safe POSIX functions.
 * 
 * Features:
 * - Lock-free ring buffer (no mutexes)
 * - Only uses async-signal-safe functions (write, open, etc.)
 * - Fixed-size buffer to avoid malloc in signal handlers
 * - Suitable for SIGSEGV, SIGABRT, etc. handlers
 * 
 * Limitations:
 * - Fixed buffer size (messages may be dropped if buffer is full)
 * - Simple formatting (no timestamps, no colors)
 * - Linux/POSIX only
 * 
 * Example usage in crash handler:
 * @code
 * auto crash_sink = std::make_shared<SignalSafeSink>("/var/log/crash.log");
 * auto logger = std::make_shared<Logger>("crash");
 * logger->add_sink(crash_sink);
 * 
 * void signal_handler(int sig) {
 *     logger->log(LogLevel::Critical, "Caught signal");
 *     crash_sink->flush();
 *     _exit(1);
 * }
 * @endcode
 */
class SignalSafeSink : public LogSink {
public:
    /**
     * @brief Construct signal-safe sink
     * @param path Path to log file (should be opened at construction)
     * @param buffer_size Size of ring buffer (default 64KB)
     */
    explicit SignalSafeSink(const std::string& path, size_t buffer_size = 65536);
    
    /**
     * @brief Destructor - closes file descriptor
     */
    ~SignalSafeSink();
    
    /**
     * @brief Log a message (async-signal-safe)
     * @param name Logger name
     * @param level Log level
     * @param message Log message
     */
    void log(const std::string& name, LogLevel level, const std::string& message) override;
    
    /**
     * @brief Flush buffer to disk (async-signal-safe)
     */
    void flush();
    
    /**
     * @brief Check if sink is ready
     */
    bool is_ready() const { return fd_ >= 0; }

private:
    int fd_;  // File descriptor (safer than FILE* in signal handlers)
    
    // Lock-free ring buffer
    static constexpr size_t MAX_BUFFER_SIZE = 1048576; // 1MB max
    std::array<char, MAX_BUFFER_SIZE> buffer_;
    std::atomic<size_t> write_pos_{0};
    std::atomic<size_t> read_pos_{0};
    size_t capacity_;
    
    // Helper functions (all async-signal-safe)
    void write_to_buffer(const char* data, size_t len);
    void flush_buffer();
    static void safe_write(int fd, const char* data, size_t len);
    static size_t safe_strlen(const char* str);
    static void safe_strcpy(char* dest, const char* src, size_t max_len);
};

} // namespace xlog
