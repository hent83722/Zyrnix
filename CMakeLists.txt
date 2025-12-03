cmake_minimum_required(VERSION 3.16)
project(xlog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories for all headers
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/xlog
    ${PROJECT_SOURCE_DIR}/include/xlog/async
    ${PROJECT_SOURCE_DIR}/include/xlog/sinks
    ${PROJECT_SOURCE_DIR}/include/xlog/experimental
    ${PROJECT_SOURCE_DIR}/include/xlog/fmt_compat
)
find_path(ASIO_INCLUDE_DIR asio.hpp)
include_directories(${ASIO_INCLUDE_DIR})
# Collect all source files
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/async/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/sinks/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/experimental/*.cpp"
)

# Add executable
add_executable(xlog ${SOURCES})

# Link pthreads
find_package(Threads REQUIRED)
target_link_libraries(xlog Threads::Threads)

# If using asio standalone, make sure to link it (optional)
# find_package(ASIO REQUIRED)
# target_link_libraries(xlog ASIO::ASIO)

# Optionally, link nlohmann JSON if you use json_sink
# find_package(nlohmann_json REQUIRED)
# target_link_libraries(xlog nlohmann_json::nlohmann_json)
