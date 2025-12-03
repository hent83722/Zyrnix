cmake_minimum_required(VERSION 3.18)
project(xlog VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)

# Dependencies
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

# Library sources
file(GLOB_RECURSE XLOG_HEADERS include/xlog/*.hpp include/xlog/**/*.hpp)
file(GLOB_RECURSE XLOG_SOURCES src/*.cpp src/**/*.cpp)

add_library(xlog STATIC ${XLOG_SOURCES} ${XLOG_HEADERS})
target_include_directories(xlog PUBLIC include)
target_link_libraries(xlog PUBLIC fmt::fmt Threads::Threads)

# Examples
if(BUILD_EXAMPLES)
    file(GLOB EXAMPLES "examples/*.cpp")
    foreach(example ${EXAMPLES})
        get_filename_component(EX_NAME ${example} NAME_WE)
        add_executable(${EX_NAME} ${example})
        target_link_libraries(${EX_NAME} PRIVATE xlog fmt::fmt Threads::Threads)
    endforeach()
endif()

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Include directories
target_include_directories(xlog
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/xlog
        ${PROJECT_SOURCE_DIR}/include/xlog/sinks
        ${PROJECT_SOURCE_DIR}/include/xlog/async
        ${PROJECT_SOURCE_DIR}/include/xlog/experimental
        ${PROJECT_SOURCE_DIR}/include/xlog/fmt_compat
)
