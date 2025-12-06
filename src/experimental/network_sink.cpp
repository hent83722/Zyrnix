#include "xlog/experimental/network_sink.hpp"

namespace xlog {

NetworkSink::NetworkSink(const std::string& host_port) : sockfd(-1) {
    auto colon = host_port.find(':');
    if (colon == std::string::npos) {
        throw std::runtime_error("host:port expected");
    }

    std::string host = host_port.substr(0, colon);
    int port = std::stoi(host_port.substr(colon + 1));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) throw std::runtime_error("Failed to create socket");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address");
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error("Connection failed");
    }
}

NetworkSink::~NetworkSink() {
    if (sockfd >= 0) close(sockfd);
}

void NetworkSink::log(const std::string& logger_name, LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string out = "[" + logger_name + "] " + std::to_string(static_cast<int>(level)) + ": " + message + "\n";
    send(sockfd, out.c_str(), out.size(), 0);
}

}
