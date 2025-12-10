#include "xlog/sinks/compressed_file_sink.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>

#ifdef XLOG_HAS_ZLIB
#include <zlib.h>
#endif

#ifdef XLOG_HAS_ZSTD
#include <zstd.h>
#endif

namespace xlog {

CompressedFileSink::CompressedFileSink(
    const std::string& filename,
    size_t max_size,
    size_t max_files,
    const CompressionOptions& options
)
    : base_filename_(filename)
    , max_size_(max_size)
    , max_files_(max_files)
    , options_(options)
    , current_size_(0)
    , files_compressed_(0)
    , original_bytes_(0)
    , compressed_bytes_(0)
{
    file_.open(base_filename_, std::ios::app);
    if (file_.is_open()) {
        file_.seekp(0, std::ios::end);
        current_size_ = file_.tellp();
    }
}

CompressedFileSink::~CompressedFileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CompressedFileSink::log(const std::string& name, LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!file_.is_open()) {
        return;
    }

    std::string formatted = formatter.format(name, level, message);
    file_ << formatted << '\n';
    current_size_ += formatted.size() + 1;

    if (current_size_ >= max_size_) {
        rotate();
    }
}

void CompressedFileSink::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.flush();
    }
}

void CompressedFileSink::rotate() {
    if (file_.is_open()) {
        file_.close();
    }

    if (max_files_ > 0) {
        std::string oldest = get_rotated_filename(max_files_);
        std::remove(oldest.c_str());
    }

    for (size_t i = max_files_; i > 0; --i) {
        std::string old_name = (i == 1) ? base_filename_ : get_rotated_filename(i - 1);
        std::string new_name = get_rotated_filename(i);
        std::rename(old_name.c_str(), new_name.c_str());
    }

    if (options_.compress_on_rotate && options_.type != CompressionType::None) {
        std::string source = get_rotated_filename(1);
        std::string dest = source + get_compressed_extension();
        
        size_t original_size = CompressionUtils::get_file_size(source);
        compress_file(source, dest);
        size_t compressed_size = CompressionUtils::get_file_size(dest);
        
        if (compressed_size > 0) {
            std::remove(source.c_str()); // Remove uncompressed file
            
            std::lock_guard<std::mutex> stats_lock(stats_mutex_);
            files_compressed_++;
            original_bytes_ += original_size;
            compressed_bytes_ += compressed_size;
        }
    }

    file_.open(base_filename_, std::ios::trunc);
    current_size_ = 0;
}

void CompressedFileSink::compress_file(const std::string& source_path, const std::string& dest_path) {
    bool success = false;
    
    switch (options_.type) {
        case CompressionType::Gzip:
            success = compress_gzip(source_path, dest_path);
            break;
        case CompressionType::Zstd:
            success = compress_zstd(source_path, dest_path);
            break;
        default:
            break;
    }
    
    (void)success; // Suppress unused variable warning
}

bool CompressedFileSink::compress_gzip(const std::string& source, const std::string& dest) {
#ifdef XLOG_HAS_ZLIB
    std::ifstream in(source, std::ios::binary);
    if (!in) return false;

    gzFile out = gzopen(dest.c_str(), ("wb" + std::to_string(options_.level)).c_str());
    if (!out) return false;

    char buffer[8192];
    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        gzwrite(out, buffer, static_cast<unsigned>(in.gcount()));
    }

    gzclose(out);
    return true;
#else
    std::string cmd = "gzip -" + std::to_string(options_.level) + " -c \"" + source + "\" > \"" + dest + "\"";
    return std::system(cmd.c_str()) == 0;
#endif
}

bool CompressedFileSink::compress_zstd(const std::string& source, const std::string& dest) {
#ifdef XLOG_HAS_ZSTD
    std::ifstream in(source, std::ios::binary);
    if (!in) return false;

    in.seekg(0, std::ios::end);
    size_t file_size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> input_buffer(file_size);
    in.read(input_buffer.data(), file_size);

    size_t compressed_bound = ZSTD_compressBound(file_size);
    std::vector<char> output_buffer(compressed_bound);

    size_t compressed_size = ZSTD_compress(
        output_buffer.data(), compressed_bound,
        input_buffer.data(), file_size,
        options_.level
    );

    if (ZSTD_isError(compressed_size)) {
        return false;
    }

    std::ofstream out(dest, std::ios::binary);
    out.write(output_buffer.data(), compressed_size);
    
    return true;
#else
    std::string cmd = "zstd -" + std::to_string(options_.level) + " -q -f \"" + source + "\" -o \"" + dest + "\"";
    return std::system(cmd.c_str()) == 0;
#endif
}

std::string CompressedFileSink::get_rotated_filename(size_t index) const {
    return base_filename_ + "." + std::to_string(index);
}

std::string CompressedFileSink::get_compressed_extension() const {
    switch (options_.type) {
        case CompressionType::Gzip:
            return ".gz";
        case CompressionType::Zstd:
            return ".zst";
        default:
            return "";
    }
}

CompressedFileSink::CompressionStats CompressedFileSink::get_compression_stats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    
    CompressionStats stats;
    stats.files_compressed = files_compressed_;
    stats.original_bytes = original_bytes_;
    stats.compressed_bytes = compressed_bytes_;
    stats.compression_ratio = (compressed_bytes_ > 0) 
        ? static_cast<double>(original_bytes_) / static_cast<double>(compressed_bytes_)
        : 0.0;
    
    return stats;
}

bool CompressionUtils::compress_file_gzip(
    const std::string& source_path,
    const std::string& dest_path,
    int level
) {
#ifdef XLOG_HAS_ZLIB
    std::ifstream in(source_path, std::ios::binary);
    if (!in) return false;

    gzFile out = gzopen(dest_path.c_str(), ("wb" + std::to_string(level)).c_str());
    if (!out) return false;

    char buffer[8192];
    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        gzwrite(out, buffer, static_cast<unsigned>(in.gcount()));
    }

    gzclose(out);
    return true;
#else
    std::string cmd = "gzip -" + std::to_string(level) + " -c \"" + source_path + "\" > \"" + dest_path + "\"";
    return std::system(cmd.c_str()) == 0;
#endif
}

bool CompressionUtils::compress_file_zstd(
    const std::string& source_path,
    const std::string& dest_path,
    int level
) {
#ifdef XLOG_HAS_ZSTD
    std::ifstream in(source_path, std::ios::binary);
    if (!in) return false;

    in.seekg(0, std::ios::end);
    size_t file_size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> input_buffer(file_size);
    in.read(input_buffer.data(), file_size);

    size_t compressed_bound = ZSTD_compressBound(file_size);
    std::vector<char> output_buffer(compressed_bound);

    size_t compressed_size = ZSTD_compress(
        output_buffer.data(), compressed_bound,
        input_buffer.data(), file_size,
        level
    );

    if (ZSTD_isError(compressed_size)) {
        return false;
    }

    std::ofstream out(dest_path, std::ios::binary);
    out.write(output_buffer.data(), compressed_size);
    
    return true;
#else
    std::string cmd = "zstd -" + std::to_string(level) + " -q -f \"" + source_path + "\" -o \"" + dest_path + "\"";
    return std::system(cmd.c_str()) == 0;
#endif
}

size_t CompressionUtils::get_file_size(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return st.st_size;
    }
    return 0;
}

bool CompressionUtils::is_gzip_available() {
#ifdef XLOG_HAS_ZLIB
    return true;
#else
    return std::system("which gzip > /dev/null 2>&1") == 0;
#endif
}

bool CompressionUtils::is_zstd_available() {
#ifdef XLOG_HAS_ZSTD
    return true;
#else
    return std::system("which zstd > /dev/null 2>&1") == 0;
#endif
}

} // namespace xlog
