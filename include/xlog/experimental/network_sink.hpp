#pragma once
#include "../log_sink.hpp"
#include <string>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace xlog {

class NetworkSink : public LogSink {
public:
    NetworkSink(const std::string& host_port);
    ~NetworkSink();

    void log(const std::string& logger_name, LogLevel level, const std::string& message) override;

private:
    int sockfd;
    std::mutex mtx;
};

}
