#pragma once

/**
 * @file xlog_features.hpp
 * @brief Feature flags for conditional compilation
 * 
 * Use these flags to disable specific features and reduce binary size.
 * Define these macros before including xlog headers or in your build system.
 * 
 * Example CMake usage:
 * @code
 * target_compile_definitions(my_app PRIVATE XLOG_NO_ASYNC XLOG_NO_JSON)
 * @endcode
 * 
 * Example compile command:
 * @code
 * g++ -DXLOG_NO_ASYNC -DXLOG_NO_JSON main.cpp -lxlog
 * @endcode
 */

// XLOG_NO_ASYNC - Disable asynchronous logging support
// Removes: ThreadPool, AsyncLogger, async sinks
// Binary size reduction: ~15-20KB
// #define XLOG_NO_ASYNC

// XLOG_NO_JSON - Disable JSON/structured logging support
// Removes: StructuredLogger, JSON formatting
// Binary size reduction: ~10-15KB
// #define XLOG_NO_JSON

// XLOG_NO_STRUCTURED - Disable structured logging (alias for XLOG_NO_JSON)
// #define XLOG_NO_STRUCTURED

// XLOG_NO_NETWORK - Disable network sinks (UDP, TCP, Syslog)
// Removes: UdpSink, NetworkSink, SyslogSink
// Binary size reduction: ~8-12KB
// #define XLOG_NO_NETWORK

// XLOG_NO_COLORS - Disable color output support
// Removes: Color formatting and ANSI escape codes
// Binary size reduction: ~2-3KB
// #define XLOG_NO_COLORS

// XLOG_NO_FILE_ROTATION - Disable rotating file sink
// Removes: RotatingFileSink
// Binary size reduction: ~5-8KB
// #define XLOG_NO_FILE_ROTATION

// XLOG_NO_CONTEXT - Disable log context (MDC/NDC) support
// Removes: LogContext, ScopedContext
// Binary size reduction: ~3-5KB
// #define XLOG_NO_CONTEXT

// XLOG_NO_FILTERS - Disable log filtering support
// Removes: LogFilter and dynamic filtering
// Binary size reduction: ~2-4KB
// #define XLOG_NO_FILTERS

// XLOG_MINIMAL - Enable all size optimizations
// Equivalent to defining all XLOG_NO_* flags
// Binary size reduction: ~50-70KB
#ifdef XLOG_MINIMAL
    #define XLOG_NO_ASYNC
    #define XLOG_NO_JSON
    #define XLOG_NO_NETWORK
    #define XLOG_NO_COLORS
    #define XLOG_NO_FILE_ROTATION
    #define XLOG_NO_CONTEXT
    #define XLOG_NO_FILTERS
#endif

// Compatibility alias
#ifdef XLOG_NO_STRUCTURED
    #define XLOG_NO_JSON
#endif

// Feature detection macros (for user code)
#ifndef XLOG_NO_ASYNC
    #define XLOG_HAS_ASYNC 1
#else
    #define XLOG_HAS_ASYNC 0
#endif

#ifndef XLOG_NO_JSON
    #define XLOG_HAS_JSON 1
#else
    #define XLOG_HAS_JSON 0
#endif

#ifndef XLOG_NO_NETWORK
    #define XLOG_HAS_NETWORK 1
#else
    #define XLOG_HAS_NETWORK 0
#endif

#ifndef XLOG_NO_COLORS
    #define XLOG_HAS_COLORS 1
#else
    #define XLOG_HAS_COLORS 0
#endif

#ifndef XLOG_NO_FILE_ROTATION
    #define XLOG_HAS_FILE_ROTATION 1
#else
    #define XLOG_HAS_FILE_ROTATION 0
#endif

#ifndef XLOG_NO_CONTEXT
    #define XLOG_HAS_CONTEXT 1
#else
    #define XLOG_HAS_CONTEXT 0
#endif

#ifndef XLOG_NO_FILTERS
    #define XLOG_HAS_FILTERS 1
#else
    #define XLOG_HAS_FILTERS 0
#endif
