#pragma once
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include "xlog/config.hpp"
#include "xlog/config_watcher.hpp"

namespace xlog {

class HotReloadManager {
public:
    HotReloadManager(const std::string& config_path);
    ~HotReloadManager();
    void start();
    void stop();
    std::shared_ptr<Logger> get_logger(const std::string& name);
    std::map<std::string, std::shared_ptr<Logger>> get_all_loggers();
private:
    void reload();
    std::string config_path_;
    std::map<std::string, std::shared_ptr<Logger>> loggers_;
    std::mutex mtx_;
    std::unique_ptr<ConfigWatcher> watcher_;
};

}
