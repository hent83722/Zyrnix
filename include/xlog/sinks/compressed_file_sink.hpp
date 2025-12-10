#pragma once
#include "../xlog_features.hpp"
#include "../log_sink.hpp"
#include "../log_record.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <fstream>

namespace xlog {

enum class CompressionType {
    None,
    Gzip,
    Zstd
};

struct CompressionOptions {
    CompressionType type = CompressionType::Gzip;
    int level = 6; // Default compression level (balance speed/ratio)
    bool compress_on_rotate = true; // Compress when rotating files
};

class CompressedFileSink : public LogSink {
public:
    CompressedFileSink(
        const std::string& filename,
        size_t max_size = 10 * 1024 * 1024, // 10 MB default
        size_t max_files = 5,
        const CompressionOptions& options = CompressionOptions{}
    );

    ~CompressedFileSink() override;

    void log(const std::string& name, LogLevel level, const std::string& message) override;
    void flush();

    size_t current_size() const { return current_size_; }

    struct CompressionStats {
        uint64_t files_compressed;
        uint64_t original_bytes;
        uint64_t compressed_bytes;
        double compression_ratio; // original / compressed
    };

    CompressionStats get_compression_stats() const;

private:
    void rotate();
    void compress_file(const std::string& source_path, const std::string& dest_path);
    bool compress_gzip(const std::string& source, const std::string& dest);
    bool compress_zstd(const std::string& source, const std::string& dest);
    std::string get_rotated_filename(size_t index) const;
    std::string get_compressed_extension() const;

    std::string base_filename_;
    size_t max_size_;
    size_t max_files_;
    CompressionOptions options_;
    
    std::ofstream file_;
    size_t current_size_;
    
    mutable std::mutex stats_mutex_;
    uint64_t files_compressed_;
    uint64_t original_bytes_;
    uint64_t compressed_bytes_;
    
    std::mutex mutex_;
};

class CompressionUtils {
public:
    static bool compress_file_gzip(
        const std::string& source_path,
        const std::string& dest_path,
        int level = 6
    );

    static bool compress_file_zstd(
        const std::string& source_path,
        const std::string& dest_path,
        int level = 3
    );

    static size_t get_file_size(const std::string& path);

    static bool is_gzip_available();
    static bool is_zstd_available();
};

} // namespace xlog
