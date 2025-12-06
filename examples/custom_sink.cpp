
#include "xlog/logger.hpp"
#include "xlog/sinks/stdout_sink.hpp"
#include "xlog/sinks/file_sink.hpp"
#include "xlog/sinks/multi_sink.hpp"
#include "xlog/experimental/network_sink.hpp"

using namespace xlog;

int main() {
    auto stdout_sink = std::make_shared<StdoutSink>();
    auto file_sink = std::make_shared<FileSink>("combined.log");
    auto network_sink = std::make_shared<NetworkSink>("127.0.0.1", 9000);

    auto multi_sink = std::make_shared<MultiSink>();
    multi_sink->add_sink(stdout_sink);
    multi_sink->add_sink(file_sink);
    multi_sink->add_sink(network_sink);

    Logger logger("MyLogger", multi_sink);

    logger.info("This is an info message!");
    logger.error("This is an error message!");

    return 0;
}
