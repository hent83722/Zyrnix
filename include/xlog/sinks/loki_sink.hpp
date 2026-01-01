#pragma once
#include "../log_sink.hpp"
#include <string>
#include <vector>
#include <mutex>
#include <memory>

namespace xlog {

class LokiSink : public LogSink {
public:
    LokiSink(const std::string& url, const std::string& labels = "");
    void log(const std::string& name, LogLevel level, const std::string& message) override;
    void flush();
    const char* name_str() const noexcept { return "LokiSink"; }

private:
    std::string url_;
    std::string labels_;
    std::vector<std::string> buffer_;
    std::mutex mutex_;
    void send_batch();
};

using LokiSinkPtr = std::shared_ptr<LokiSink>;

} // namespace xlog
