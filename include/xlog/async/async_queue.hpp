#pragma once
#include "log_record.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>

namespace xlog {

class AsyncQueue {
public:
    void push(LogRecord&& record);
    bool pop(LogRecord& record);
    bool empty() const;

private:
    std::queue<LogRecord> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
};

}
