#pragma once
#include "logger.hpp"
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>

namespace xlog {

class AsyncLogger {
public:
    explicit AsyncLogger(LoggerPtr logger);
    ~AsyncLogger();
    void log(LogLevel level, const std::string& message);

private:
    void worker();
    LoggerPtr logger;
    std::queue<std::pair<LogLevel, std::string>> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread thread;
    std::atomic<bool> running;
};

}
