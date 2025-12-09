cmake_minimum_required(VERSION 3.16)
project(xlog VERSION 1.0.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Feature flags for conditional compilation (reduce binary size)
option(XLOG_ENABLE_ASYNC "Enable asynchronous logging support" ON)
option(XLOG_ENABLE_JSON "Enable JSON/structured logging support" ON)
option(XLOG_ENABLE_NETWORK "Enable network sinks (UDP, TCP, Syslog)" ON)
option(XLOG_ENABLE_COLORS "Enable color output support" ON)
option(XLOG_ENABLE_FILE_ROTATION "Enable rotating file sink" ON)
option(XLOG_ENABLE_CONTEXT "Enable log context (MDC/NDC) support" ON)
option(XLOG_ENABLE_FILTERS "Enable log filtering support" ON)
option(XLOG_MINIMAL "Enable minimal build (disable all optional features)" OFF)

option(ENABLE_SYSLOG "Enable Syslog sink (Unix/Linux only)" ON)

# On Windows, disable syslog by default
if(WIN32)
    set(ENABLE_SYSLOG OFF CACHE BOOL "Enable Syslog sink (Unix/Linux only)" FORCE)
endif()

# Apply minimal build settings
if(XLOG_MINIMAL)
    set(XLOG_ENABLE_ASYNC OFF)
    set(XLOG_ENABLE_JSON OFF)
    set(XLOG_ENABLE_NETWORK OFF)
    set(XLOG_ENABLE_COLORS OFF)
    set(XLOG_ENABLE_FILE_ROTATION OFF)
    set(XLOG_ENABLE_CONTEXT OFF)
    set(XLOG_ENABLE_FILTERS OFF)
endif()

file(GLOB XLOG_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/*.cpp"
)

# Conditionally add async sources
if(XLOG_ENABLE_ASYNC)
    file(GLOB ASYNC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/async/*.cpp")
    list(APPEND XLOG_SOURCES ${ASYNC_SOURCES})
else()
    target_compile_definitions(xlog PUBLIC XLOG_NO_ASYNC)
endif()

# Conditionally add structured/JSON logging
if(NOT XLOG_ENABLE_JSON)
    list(REMOVE_ITEM XLOG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/structured_logger.cpp")
    target_compile_definitions(xlog PUBLIC XLOG_NO_JSON)
endif()

# Conditionally add network sinks
if(NOT XLOG_ENABLE_NETWORK)
    list(REMOVE_ITEM XLOG_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/udp_sink.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/network_sink.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/syslog_sink.cpp")
    target_compile_definitions(xlog PUBLIC XLOG_NO_NETWORK)
endif()

# Conditionally add experimental features
if(XLOG_ENABLE_JSON)
    file(GLOB EXPERIMENTAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/experimental/*.cpp")
    list(APPEND XLOG_SOURCES ${EXPERIMENTAL_SOURCES})
endif()

# Remove syslog_sink.cpp if ENABLE_SYSLOG is OFF
if(NOT ENABLE_SYSLOG)
    list(REMOVE_ITEM XLOG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/syslog_sink.cpp")
endif()

# Conditionally remove color support
if(NOT XLOG_ENABLE_COLORS)
    target_compile_definitions(xlog PUBLIC XLOG_NO_COLORS)
endif()

# Conditionally remove rotating file sink
if(NOT XLOG_ENABLE_FILE_ROTATION)
    list(REMOVE_ITEM XLOG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sinks/rotating_file_sink.cpp")
    target_compile_definitions(xlog PUBLIC XLOG_NO_FILE_ROTATION)
endif()

# Conditionally remove context support
if(NOT XLOG_ENABLE_CONTEXT)
    list(REMOVE_ITEM XLOG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/log_context.cpp")
    target_compile_definitions(xlog PUBLIC XLOG_NO_CONTEXT)
endif()

# Conditionally remove filter support
if(NOT XLOG_ENABLE_FILTERS)
    list(REMOVE_ITEM XLOG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/log_filter.cpp")
    target_compile_definitions(xlog PUBLIC XLOG_NO_FILTERS)
endif()

add_library(xlog STATIC ${XLOG_SOURCES})

target_include_directories(xlog PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/xlog>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS xlog
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/xlogConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/xlogConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/xlogConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/xlogConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/xlogConfigVersion.cmake"
    DESTINATION lib/cmake/xlog
)
