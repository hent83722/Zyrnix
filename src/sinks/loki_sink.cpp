#include <xlog/sinks/loki_sink.hpp>
#include <xlog/formatter.hpp>
#include <xlog/log_message.hpp>
#include <sstream>
#include <curl/curl.h>

namespace xlog {

LokiSink::LokiSink(const std::string& url, const std::string& labels)
    : url_(url), labels_(labels) {}

void LokiSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    auto now = std::chrono::system_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    oss << "{";
    oss << "\"ts\":\"" << ts << "\",";
    oss << "\"line\":\"" << message << "\"}";
    buffer_.push_back(oss.str());
    if (buffer_.size() >= 10) send_batch();
}

void LokiSink::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!buffer_.empty()) send_batch();
}

void LokiSink::send_batch() {
    if (buffer_.empty()) return;
    std::ostringstream payload;
    payload << "{" << "\"streams\":[{" << "\"labels\":\"" << labels_ << "\"," << "\"entries\":[";
    for (size_t i = 0; i < buffer_.size(); ++i) {
        if (i > 0) payload << ",";
        payload << buffer_[i];
    }
    payload << "]}]}";
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.str().c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    buffer_.clear();
}

} // namespace xlog
