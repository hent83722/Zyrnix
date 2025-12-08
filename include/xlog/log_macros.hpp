#pragma once
#include "logger.hpp"
#include "log_level.hpp"

#ifndef XLOG_ACTIVE_LEVEL
    #ifdef NDEBUG
        #define XLOG_ACTIVE_LEVEL 2
    #else
        #define XLOG_ACTIVE_LEVEL 0
    #endif
#endif

#define XLOG_LEVEL_ENABLED(logger, level) \
    ((int)(level) >= XLOG_ACTIVE_LEVEL && (logger)->get_level() <= (level))

#define XLOG_LOG_IF(logger, level, condition, message) \
    do { \
        if (XLOG_LEVEL_ENABLED(logger, level) && (condition)) { \
            (logger)->log(level, message); \
        } \
    } while(0)

#define XLOG_TRACE_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Trace, condition, message)

#define XLOG_DEBUG_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Debug, condition, message)

#define XLOG_INFO_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Info, condition, message)

#define XLOG_WARN_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Warn, condition, message)

#define XLOG_ERROR_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Error, condition, message)

#define XLOG_CRITICAL_IF(logger, condition, message) \
    XLOG_LOG_IF(logger, ::xlog::LogLevel::Critical, condition, message)

#if XLOG_ACTIVE_LEVEL <= 0
    #define XLOG_TRACE(logger, message) (logger)->log(::xlog::LogLevel::Trace, message)
#else
    #define XLOG_TRACE(logger, message) ((void)0)
#endif

#if XLOG_ACTIVE_LEVEL <= 1
    #define XLOG_DEBUG(logger, message) (logger)->log(::xlog::LogLevel::Debug, message)
#else
    #define XLOG_DEBUG(logger, message) ((void)0)
#endif

#if XLOG_ACTIVE_LEVEL <= 2
    #define XLOG_INFO(logger, message) (logger)->log(::xlog::LogLevel::Info, message)
#else
    #define XLOG_INFO(logger, message) ((void)0)
#endif

#if XLOG_ACTIVE_LEVEL <= 3
    #define XLOG_WARN(logger, message) (logger)->log(::xlog::LogLevel::Warn, message)
#else
    #define XLOG_WARN(logger, message) ((void)0)
#endif

#if XLOG_ACTIVE_LEVEL <= 4
    #define XLOG_ERROR(logger, message) (logger)->log(::xlog::LogLevel::Error, message)
#else
    #define XLOG_ERROR(logger, message) ((void)0)
#endif

#if XLOG_ACTIVE_LEVEL <= 5
    #define XLOG_CRITICAL(logger, message) (logger)->log(::xlog::LogLevel::Critical, message)
#else
    #define XLOG_CRITICAL(logger, message) ((void)0)
#endif
