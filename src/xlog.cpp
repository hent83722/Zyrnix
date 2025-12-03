#include "xlog/xlog.hpp"
#include "xlog/sinks/stdout_sink.hpp"
#include "xlog/sinks/file_sink.hpp"
#include "xlog/async/async_logger.hpp"
#include <memory>

namespace xlog {

LoggerPtr create_logger(const std::string& name, const Config& cfg) {
    auto stdout_sink = std::make_shared<StdoutSink>();
    auto file_sink = std::make_shared<FileSink>(cfg.output_file);
    std::vector<LogSinkPtr> sinks = { stdout_sink, file_sink };
    auto logger = std::make_shared<Logger>(name, sinks, cfg);
    return logger;
}

AsyncLoggerPtr create_async_logger(LoggerPtr logger, const Config& cfg) {
    return std::make_shared<AsyncLogger>(logger, cfg);
}

}
