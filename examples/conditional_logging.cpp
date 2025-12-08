#include <xlog/logger.hpp>
#include <xlog/log_macros.hpp>
#include <xlog/log_filter.hpp>
#include <xlog/log_context.hpp>
#include <xlog/sinks/file_sink.hpp>
#include <xlog/sinks/stdout_sink.hpp>
#include <iostream>
#include <thread>
#include <chrono>

void demo_compile_time_filtering() {
    std::cout << "\n=== Compile-Time Filtering Demo ===\n";
    
    auto logger = xlog::Logger::create_stdout_logger("compile_time");
    
    std::cout << "Using macros for zero-cost debug logs:\n";
    
    XLOG_TRACE(logger, "Trace: This might be compiled out in release");
    XLOG_DEBUG(logger, "Debug: This might be compiled out in release");
    XLOG_INFO(logger, "Info: This is always included");
    XLOG_WARN(logger, "Warning: Always included");
    XLOG_ERROR(logger, "Error: Always included");
    
    std::cout << "\nIn Release builds (-DNDEBUG), TRACE and DEBUG are eliminated at compile time!\n";
}

void demo_conditional_logging() {
    std::cout << "\n=== Conditional Logging Demo ===\n";
    
    auto logger = xlog::Logger::create_stdout_logger("conditional");
    
    int request_count = 0;
    bool is_premium_user = true;
    
    std::cout << "Only log expensive operations when condition is true:\n";
    
    for (int i = 0; i < 5; ++i) {
        request_count++;
        
        XLOG_DEBUG_IF(logger, request_count % 2 == 0, 
            "Even request: " + std::to_string(request_count));
        
        XLOG_INFO_IF(logger, is_premium_user, 
            "Premium user action: " + std::to_string(i));
    }
    
    std::cout << "\nConditional macros prevent message construction if condition is false!\n";
}

void demo_runtime_filtering() {
    std::cout << "\n=== Runtime Filtering Demo ===\n";
    
    auto logger = std::make_shared<xlog::Logger>("runtime_filter");
    logger->add_sink(std::make_shared<xlog::StdoutSink>());
    
    std::cout << "1. Level-based filtering:\n";
    logger->set_level(xlog::LogLevel::Warn);
    
    logger->info("This won't appear (below Warn level)");
    logger->warn("This will appear (Warn level)");
    logger->error("This will appear (Error level)");
    
    logger->set_level(xlog::LogLevel::Trace);
    
    std::cout << "\n2. Custom lambda filter (only log errors):\n";
    logger->set_filter_func([](const xlog::LogRecord& record) {
        return record.level >= xlog::LogLevel::Error;
    });
    
    logger->info("Filtered out (not an error)");
    logger->error("Passes filter (is an error)");
    
    logger->clear_filters();
}

void demo_field_based_filtering() {
    std::cout << "\n=== Field-Based Filtering Demo ===\n";
    
    auto logger = std::make_shared<xlog::Logger>("field_filter");
    logger->add_sink(std::make_shared<xlog::StdoutSink>());
    
    std::cout << "Only log messages with specific context fields:\n";
    
    auto filter = std::make_shared<xlog::FieldFilter>("user_type", "premium");
    logger->add_filter(filter);
    
    {
        xlog::ScopedContext ctx;
        ctx.set("user_type", "premium");
        logger->info("Premium user activity (passes filter)");
    }
    
    {
        xlog::ScopedContext ctx;
        ctx.set("user_type", "free");
        logger->info("Free user activity (filtered out)");
    }
    
    logger->clear_filters();
}

void demo_composite_filtering() {
    std::cout << "\n=== Composite Filtering Demo ===\n";
    
    auto logger = std::make_shared<xlog::Logger>("composite");
    logger->add_sink(std::make_shared<xlog::StdoutSink>());
    
    std::cout << "Combine multiple filters with AND logic:\n";
    
    auto composite = std::make_shared<xlog::CompositeFilter>(
        xlog::CompositeFilter::Mode::AND
    );
    
    composite->add_filter(std::make_shared<xlog::LevelFilter>(xlog::LogLevel::Info));
    composite->add_filter(std::make_shared<xlog::FieldFilter>("important", "true"));
    
    logger->add_filter(composite);
    
    {
        xlog::ScopedContext ctx;
        ctx.set("important", "true");
        
        logger->debug("Debug + important (filtered - level too low)");
        logger->info("Info + important (passes both filters)");
    }
    
    {
        xlog::ScopedContext ctx;
        ctx.set("important", "false");
        
        logger->info("Info + not important (filtered - wrong field)");
    }
    
    logger->clear_filters();
}

void demo_advanced_lambda_filter() {
    std::cout << "\n=== Advanced Lambda Filtering Demo ===\n";
    
    auto logger = std::make_shared<xlog::Logger>("advanced");
    logger->add_sink(std::make_shared<xlog::StdoutSink>());
    
    std::cout << "Complex filtering logic with lambdas:\n";
    
    logger->set_filter_func([](const xlog::LogRecord& record) {
        auto context = xlog::LogContext::get_all();
        
        if (record.level >= xlog::LogLevel::Error) {
            return true;
        }
        
        auto it = context.find("request_id");
        if (it != context.end() && it->second.find("urgent") != std::string::npos) {
            return true;
        }
        
        return false;
    });
    
    logger->info("Normal info (filtered out)");
    logger->error("Error message (always passes)");
    
    {
        xlog::ScopedContext ctx;
        ctx.set("request_id", "urgent-12345");
        logger->info("Urgent request info (passes due to request_id)");
    }
    
    logger->clear_filters();
}

void demo_performance_comparison() {
    std::cout << "\n=== Performance Comparison Demo ===\n";
    
    auto logger = std::make_shared<xlog::Logger>("perf");
    logger->add_sink(std::make_shared<xlog::FileSink>("/dev/null"));
    
    const int iterations = 100000;
    
    logger->set_level(xlog::LogLevel::Error);
    
    std::cout << "Running " << iterations << " filtered logs...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        logger->debug("This gets filtered at runtime");
    }
    auto runtime_filter = std::chrono::high_resolution_clock::now() - start;
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        XLOG_DEBUG(logger, "This gets eliminated at compile time");
    }
    auto compile_time = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << "Runtime filtering: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(runtime_filter).count() 
              << " μs\n";
    std::cout << "Compile-time elimination: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(compile_time).count() 
              << " μs\n";
    std::cout << "Speedup: " 
              << (float)runtime_filter.count() / compile_time.count() << "x faster!\n";
}

int main() {
    std::cout << "===========================================\n";
    std::cout << "XLog Conditional Logging & Filtering Demo\n";
    std::cout << "===========================================\n";
    
    demo_compile_time_filtering();
    demo_conditional_logging();
    demo_runtime_filtering();
    demo_field_based_filtering();
    demo_composite_filtering();
    demo_advanced_lambda_filter();
    demo_performance_comparison();
    
    std::cout << "\n===========================================\n";
    std::cout << "Demo Complete!\n";
    std::cout << "===========================================\n";
    
    return 0;
}
