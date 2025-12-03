#include "xlog/async/async_logger.hpp"
#include <utility>

namespace xlog {

AsyncLogger::AsyncLogger(LoggerPtr logger_ptr)
    : logger_(std::move(logger_ptr)),
      running_(true),
      thread_(&AsyncLogger::worker, this)
{
}

AsyncLogger::~AsyncLogger() {
    running_ = false;
    cv_.notify_all();
    if (thread_.joinable()) thread_.join();
}

void AsyncLogger::log(LogLevel level, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.emplace(level, message);
    }
    cv_.notify_one();
}

void AsyncLogger::worker() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

        while (!queue_.empty()) {
            auto [level, message] = queue_.front();
            queue_.pop();
            lock.unlock();

            logger_->log(level, message);

            lock.lock();
        }
    }
}

}
