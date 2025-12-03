#include "xlog.hpp"

int main() {
    auto logger = std::make_shared<xlog::Logger>("daily");
    auto sink = std::make_shared<xlog::DailyFileSink>("daily_log");
    logger->add_sink(sink);

    logger->info("Logging daily message");
    return 0;
}
