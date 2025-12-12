#include "xlog/sinks/signal_safe_sink.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <algorithm>

namespace xlog {

SignalSafeSink::SignalSafeSink(const std::string& path, size_t buffer_size) 
    : fd_(-1), capacity_(std::min(buffer_size, MAX_BUFFER_SIZE)) {
    

    fd_ = ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
}

SignalSafeSink::~SignalSafeSink() {
    if (fd_ >= 0) {
        flush();
        ::close(fd_);
    }
}

void SignalSafeSink::log(const std::string& name, LogLevel level, const std::string& message) {
    if (fd_ < 0) {
        return;
    }
    

    
    const char* level_str;
    switch (level) {
        case LogLevel::Trace:    level_str = "[TRACE] "; break;
        case LogLevel::Debug:    level_str = "[DEBUG] "; break;
        case LogLevel::Info:     level_str = "[INFO] "; break;
        case LogLevel::Warn:     level_str = "[WARN] "; break;
        case LogLevel::Error:    level_str = "[ERROR] "; break;
        case LogLevel::Critical: level_str = "[CRITICAL] "; break;
        default:                 level_str = "[UNKNOWN] "; break;
    }
    
 
    write_to_buffer(level_str, safe_strlen(level_str));
    
 
    write_to_buffer(message.c_str(), message.length());
    

    write_to_buffer("\n", 1);
    

    size_t current_size = write_pos_.load(std::memory_order_relaxed) - read_pos_.load(std::memory_order_relaxed);
    if (current_size > capacity_ / 2) {
        flush_buffer();
    }
}

void SignalSafeSink::flush() {
    flush_buffer();
    if (fd_ >= 0) {
        ::fsync(fd_); 
    }
}

void SignalSafeSink::write_to_buffer(const char* data, size_t len) {
    size_t write_pos = write_pos_.load(std::memory_order_relaxed);
    size_t read_pos = read_pos_.load(std::memory_order_acquire);
    
    size_t available = capacity_ - (write_pos - read_pos);
    
    if (len > available) {

        flush_buffer();
        write_pos = write_pos_.load(std::memory_order_relaxed);
        read_pos = read_pos_.load(std::memory_order_acquire);
        available = capacity_ - (write_pos - read_pos);
        
        if (len > available) {
            return; 
        }
    }
    

    for (size_t i = 0; i < len; ++i) {
        size_t idx = (write_pos + i) % capacity_;
        buffer_[idx] = data[i];
    }
    
    write_pos_.store(write_pos + len, std::memory_order_release);
}

void SignalSafeSink::flush_buffer() {
    if (fd_ < 0) {
        return;
    }
    
    size_t write_pos = write_pos_.load(std::memory_order_relaxed);
    size_t read_pos = read_pos_.load(std::memory_order_acquire);
    
    if (write_pos == read_pos) {
        return; 
    }
    

    while (read_pos < write_pos) {
        size_t idx = read_pos % capacity_;
        size_t end_idx = write_pos % capacity_;
        
        size_t chunk_size;
        if (write_pos - read_pos <= capacity_ && idx < end_idx) {
     
            chunk_size = end_idx - idx;
        } else {
   
            chunk_size = capacity_ - idx;
        }
        
        safe_write(fd_, &buffer_[idx], chunk_size);
        read_pos += chunk_size;
    }
    
    read_pos_.store(read_pos, std::memory_order_release);
}

void SignalSafeSink::safe_write(int fd, const char* data, size_t len) {
    size_t written = 0;
    while (written < len) {
        ssize_t ret = ::write(fd, data + written, len - written);
        if (ret < 0) {
            break; 
        }
        written += ret;
    }
}

size_t SignalSafeSink::safe_strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void SignalSafeSink::safe_strcpy(char* dest, const char* src, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

} // namespace xlog
