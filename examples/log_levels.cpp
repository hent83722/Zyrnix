#include "xlog.hpp"

int main() {
    auto logger = xlog::create_logger("levels");

    for (int i = 0; i <= static_cast<int>(xlog::LogLevel::Critical); ++i) {
        logger->log(static_cast<xlog::LogLevel>(i), "Test level " + std::to_string(i));
    }

    return 0;
}
