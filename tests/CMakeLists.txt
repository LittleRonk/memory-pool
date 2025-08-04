add_executable(pool_tests
    tests.c
    block_pool_tests.c
    dynamic_pool_tests.c)

target_link_libraries(pool_tests PRIVATE pool pool_logger)

enable_testing()

add_test(NAME pool_tests
    COMMAND pool_tests)
