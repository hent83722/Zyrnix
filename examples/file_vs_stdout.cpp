#include "xlog.hpp"

int main() {
    auto logger = std::make_shared<xlog::Logger>("file_vs_stdout");

    auto stdout_sink = std::make_shared<xlog::StdoutSink>();
    auto file_sink = std::make_shared<xlog::FileSink>("output.log");

    logger->add_sink(stdout_sink);
    logger->add_sink(file_sink);

    logger->info("Message to both stdout and file");
    return 0;
}
