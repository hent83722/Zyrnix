#include "xlog.hpp"

namespace xlog {

LoggerPtr create_logger(const std::string& name, const Config& cfg = Config()) {
    auto logger = std::make_shared<Logger>(name);
    auto stdout_sink = std::make_shared<StdoutSink>();
    stdout_sink->set_level(cfg.default_level);
    logger->add_sink(stdout_sink);
    return logger;
}

AsyncLoggerPtr create_async_logger(LoggerPtr logger, const Config& cfg = Config()) {
    return std::make_shared<AsyncLogger>(logger);
}

}
