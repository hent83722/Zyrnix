#include "xlog.hpp"

int main() {
    auto logger = std::make_shared<xlog::Logger>("rotating");
    auto sink = std::make_shared<xlog::RotatingFileSink>("rotating_log", 1024, 3);
    logger->add_sink(sink);

    for (int i = 0; i < 100; ++i) {
        logger->info("Rotating log entry " + std::to_string(i));
    }

    return 0;
}
