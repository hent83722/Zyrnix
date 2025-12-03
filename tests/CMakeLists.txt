file(GLOB TEST_SOURCES "*.cpp")

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE xlog fmt::fmt Threads::Threads)
enable_testing()
add_test(NAME xlog_tests COMMAND tests)
