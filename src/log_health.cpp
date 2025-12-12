#include "xlog/log_health.hpp"
#include "xlog/logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace xlog {

std::string HealthCheckResult::to_json() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    oss << "{\n";
    oss << "  \"status\": \"";
    switch (status) {
        case HealthStatus::Healthy: oss << "healthy"; break;
        case HealthStatus::Degraded: oss << "degraded"; break;
        case HealthStatus::Unhealthy: oss << "unhealthy"; break;
    }
    oss << "\",\n";
    
    oss << "  \"message\": \"" << message << "\",\n";
    
    auto time_t_val = std::chrono::system_clock::to_time_t(timestamp);
    oss << "  \"timestamp\": \"" << std::put_time(std::gmtime(&time_t_val), "%Y-%m-%dT%H:%M:%SZ") << "\",\n";
    
    oss << "  \"metrics\": {\n";
    oss << "    \"messages_logged\": " << messages_logged << ",\n";
    oss << "    \"messages_dropped\": " << messages_dropped << ",\n";
    oss << "    \"messages_filtered\": " << messages_filtered << ",\n";
    oss << "    \"errors\": " << errors << ",\n";
    oss << "    \"messages_per_second\": " << messages_per_second << ",\n";
    oss << "    \"avg_latency_us\": " << avg_latency_us << ",\n";
    oss << "    \"max_latency_us\": " << max_latency_us << ",\n";
    oss << "    \"queue_depth\": " << queue_depth << ",\n";
    oss << "    \"max_queue_depth\": " << max_queue_depth << "\n";
    oss << "  },\n";
    
    oss << "  \"indicators\": {\n";
    oss << "    \"drop_rate\": " << (drop_rate * 100.0) << ",\n";
    oss << "    \"error_rate\": " << (error_rate * 100.0) << ",\n";
    oss << "    \"queue_full_warning\": " << (queue_full_warning ? "true" : "false") << ",\n";
    oss << "    \"high_latency_warning\": " << (high_latency_warning ? "true" : "false") << "\n";
    oss << "  }\n";
    oss << "}";
    
    return oss.str();
}

std::string HealthCheckResult::to_string() const {
    std::ostringstream oss;
    
    oss << "Health Status: ";
    switch (status) {
        case HealthStatus::Healthy: oss << "HEALTHY"; break;
        case HealthStatus::Degraded: oss << "DEGRADED"; break;
        case HealthStatus::Unhealthy: oss << "UNHEALTHY"; break;
    }
    oss << "\n";
    
    oss << "Message: " << message << "\n";
    oss << "Messages Logged: " << messages_logged << "\n";
    oss << "Messages Dropped: " << messages_dropped << " (" << (drop_rate * 100.0) << "%)\n";
    oss << "Messages Filtered: " << messages_filtered << "\n";
    oss << "Errors: " << errors << " (" << (error_rate * 100.0) << "%)\n";
    oss << "Throughput: " << messages_per_second << " msg/sec\n";
    oss << "Avg Latency: " << avg_latency_us << " μs\n";
    oss << "Max Latency: " << max_latency_us << " μs\n";
    oss << "Queue Depth: " << queue_depth << "/" << max_queue_depth << "\n";
    
    if (queue_full_warning) {
        oss << "⚠️  Warning: Queue near capacity\n";
    }
    if (high_latency_warning) {
        oss << "⚠️  Warning: High latency detected\n";
    }
    
    return oss.str();
}

HealthChecker::HealthChecker(const HealthCheckConfig& config)
    : config_(config) {}

HealthCheckResult HealthChecker::check_logger(const Logger& logger, const LogMetrics& metrics) const {
    return check_metrics(metrics, 10000);
}

HealthCheckResult HealthChecker::check_metrics(const LogMetrics& metrics, size_t queue_capacity) const {
    HealthCheckResult result;
    result.timestamp = std::chrono::system_clock::now();
    

    auto snapshot = metrics.get_snapshot();
    result.messages_logged = snapshot.messages_logged;
    result.messages_dropped = snapshot.messages_dropped;
    result.messages_filtered = snapshot.messages_filtered;
    result.errors = snapshot.errors;
    result.messages_per_second = snapshot.messages_per_second;
    result.avg_latency_us = snapshot.avg_log_latency_us;
    result.max_latency_us = snapshot.max_log_latency_us;
    result.queue_depth = snapshot.current_queue_depth;
    result.max_queue_depth = snapshot.max_queue_depth;
    

    uint64_t total_attempts = result.messages_logged + result.messages_dropped;
    result.drop_rate = total_attempts > 0 ? static_cast<double>(result.messages_dropped) / total_attempts : 0.0;
    result.error_rate = result.messages_logged > 0 ? static_cast<double>(result.errors) / result.messages_logged : 0.0;
    
  
    double queue_usage = queue_capacity > 0 ? static_cast<double>(result.queue_depth) / queue_capacity : 0.0;
    result.queue_full_warning = queue_usage >= config_.max_queue_usage_degraded;
    result.high_latency_warning = result.max_latency_us >= config_.max_latency_us_degraded;
    

    result.status = determine_status(
        result.drop_rate,
        result.error_rate,
        result.max_latency_us,
        queue_usage
    );
    
    
    std::ostringstream msg;
    switch (result.status) {
        case HealthStatus::Healthy:
            msg << "All systems operational";
            break;
        case HealthStatus::Degraded:
            msg << "Performance degraded: ";
            if (result.drop_rate > config_.max_drop_rate_healthy) {
                msg << "high drop rate (" << (result.drop_rate * 100.0) << "%) ";
            }
            if (result.error_rate > config_.max_error_rate_healthy) {
                msg << "high error rate (" << (result.error_rate * 100.0) << "%) ";
            }
            if (result.max_latency_us > config_.max_latency_us_healthy) {
                msg << "high latency (" << result.max_latency_us << "μs) ";
            }
            if (queue_usage > config_.max_queue_usage_healthy) {
                msg << "queue usage (" << (queue_usage * 100.0) << "%) ";
            }
            break;
        case HealthStatus::Unhealthy:
            msg << "Critical issues detected: ";
            if (result.drop_rate > config_.max_drop_rate_degraded) {
                msg << "critical drop rate (" << (result.drop_rate * 100.0) << "%) ";
            }
            if (result.error_rate > config_.max_error_rate_degraded) {
                msg << "critical error rate (" << (result.error_rate * 100.0) << "%) ";
            }
            if (result.max_latency_us > config_.max_latency_us_degraded) {
                msg << "critical latency (" << result.max_latency_us << "μs) ";
            }
            if (queue_usage > config_.max_queue_usage_degraded) {
                msg << "queue near capacity (" << (queue_usage * 100.0) << "%) ";
            }
            break;
    }
    result.message = msg.str();
    
    return result;
}

void HealthChecker::set_config(const HealthCheckConfig& config) {
    config_ = config;
}

HealthStatus HealthChecker::determine_status(
    double drop_rate,
    double error_rate,
    uint64_t max_latency,
    double queue_usage) const {
    

    if (drop_rate > config_.max_drop_rate_degraded ||
        error_rate > config_.max_error_rate_degraded ||
        max_latency > config_.max_latency_us_degraded ||
        queue_usage > config_.max_queue_usage_degraded) {
        return HealthStatus::Unhealthy;
    }
    
 
    if (drop_rate > config_.max_drop_rate_healthy ||
        error_rate > config_.max_error_rate_healthy ||
        max_latency > config_.max_latency_us_healthy ||
        queue_usage > config_.max_queue_usage_healthy) {
        return HealthStatus::Degraded;
    }
    
    return HealthStatus::Healthy;
}

HealthRegistry& HealthRegistry::instance() {
    static HealthRegistry registry;
    return registry;
}

void HealthRegistry::register_logger(const std::string& name, std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_[name] = logger;
}

void HealthRegistry::unregister_logger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.erase(name);
}

HealthCheckResult HealthRegistry::check_logger(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loggers_.find(name);
    if (it == loggers_.end()) {
        HealthCheckResult result;
        result.status = HealthStatus::Unhealthy;
        result.message = "Logger not found: " + name;
        result.timestamp = std::chrono::system_clock::now();
        return result;
    }
    
    auto logger = it->second.lock();
    if (!logger) {
        HealthCheckResult result;
        result.status = HealthStatus::Unhealthy;
        result.message = "Logger expired: " + name;
        result.timestamp = std::chrono::system_clock::now();
        return result;
    }
    

    LogMetrics metrics;
    return health_checker_->check_logger(*logger, metrics);
}

std::map<std::string, HealthCheckResult> HealthRegistry::check_all() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, HealthCheckResult> results;
    for (const auto& [name, weak_logger] : loggers_) {
        auto logger = weak_logger.lock();
        if (logger) {
            LogMetrics metrics;
            results[name] = health_checker_->check_logger(*logger, metrics);
        }
    }
    
    return results;
}

std::string HealthRegistry::export_json() const {
    auto results = check_all();
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"overall_status\": \"";
    
    auto overall = get_overall_status();
    switch (overall) {
        case HealthStatus::Healthy: oss << "healthy"; break;
        case HealthStatus::Degraded: oss << "degraded"; break;
        case HealthStatus::Unhealthy: oss << "unhealthy"; break;
    }
    oss << "\",\n";
    
    oss << "  \"loggers\": [\n";
    bool first = true;
    for (const auto& [name, result] : results) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "    {\n";
        oss << "      \"name\": \"" << name << "\",\n";
        

        std::string result_json = result.to_json();
        std::istringstream iss(result_json);
        std::string line;
        bool first_line = true;
        while (std::getline(iss, line)) {
            if (!first_line) oss << "\n";
            if (!line.empty()) {
                oss << "      " << line;
            }
            first_line = false;
        }
        oss << "\n    }";
    }
    oss << "\n  ]\n";
    oss << "}";
    
    return oss.str();
}

HealthStatus HealthRegistry::get_overall_status() const {
    auto results = check_all();
    
    if (results.empty()) {
        return HealthStatus::Healthy;
    }
    
    bool has_unhealthy = false;
    bool has_degraded = false;
    
    for (const auto& [name, result] : results) {
        if (result.status == HealthStatus::Unhealthy) {
            has_unhealthy = true;
        } else if (result.status == HealthStatus::Degraded) {
            has_degraded = true;
        }
    }
    
    if (has_unhealthy) {
        return HealthStatus::Unhealthy;
    } else if (has_degraded) {
        return HealthStatus::Degraded;
    } else {
        return HealthStatus::Healthy;
    }
}

void HealthRegistry::set_health_checker(std::shared_ptr<HealthChecker> checker) {
    std::lock_guard<std::mutex> lock(mutex_);
    health_checker_ = std::move(checker);
}

} // namespace xlog
