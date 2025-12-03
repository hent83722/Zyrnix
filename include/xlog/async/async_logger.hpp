#pragma once
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include "xlog/logger.hpp"
#include "xlog/async/async_queue.hpp"
#include "xlog/async/thread_pool.hpp"
#include "xlog/config.hpp"

namespace xlog {

class AsyncLogger {
public:
    AsyncLogger(LoggerPtr logger, const Config& cfg);
    void push(LogMessage&& msg);
    void flush();
private:
    LoggerPtr logger_;
    AsyncQueue queue_;
    ThreadPool pool_;
    std::atomic<bool> running_;
    void worker_loop();
};

using AsyncLoggerPtr = std::shared_ptr<AsyncLogger>;

}
