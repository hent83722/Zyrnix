#pragma once
#include <memory>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <utility>
#include "xlog/logger.hpp"
#include "xlog/log_level.hpp"

namespace xlog {

class AsyncLogger {
public:
    explicit AsyncLogger(LoggerPtr logger);
    ~AsyncLogger();
    void log(LogLevel level, const std::string& message);

private:
    void worker();
    LoggerPtr logger_;
    std::queue<std::pair<LogLevel, std::string>> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread thread_;
    std::atomic<bool> running_;
};

using AsyncLoggerPtr = std::shared_ptr<AsyncLogger>;

}
