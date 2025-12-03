#include "async_logger.hpp"

namespace xlog {

AsyncLogger::AsyncLogger(LoggerPtr logger_ptr) : logger(std::move(logger_ptr)), running(true), thread(&AsyncLogger::worker, this) {}

AsyncLogger::~AsyncLogger() {
    running = false;
    cv.notify_all();
    if (thread.joinable()) thread.join();
}

void AsyncLogger::log(LogLevel level, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.emplace(level, message);
    }
    cv.notify_one();
}

void AsyncLogger::worker() {
    while (running) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !queue.empty() || !running; });
        while (!queue.empty()) {
            auto [level, message] = queue.front();
            queue.pop();
            lock.unlock();
            logger->log(level, message);
            lock.lock();
        }
    }
}

}
