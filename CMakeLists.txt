cmake_minimum_required(VERSION 3.14)
project(xlog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE XLOG_SOURCES
    src/*.cpp
)

add_library(xlog STATIC ${XLOG_SOURCES})

target_include_directories(xlog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/xlog
)

target_link_libraries(xlog pthread)