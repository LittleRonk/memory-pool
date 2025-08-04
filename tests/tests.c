#include <stdio.h>
#include <pool_logger.h>
#include "tests.h"

int main() {
    poolEnableLogToStdout(LOG_LEVEL_WARN);
    poolEnableLogToFile("log.txt", LOG_LEVEL_DEBUG);
    // Block pool tests
    test_block_pool_basic();
    test_block_pool_overflow();
    test_block_pool_invalid_free();
    test_block_pool_alignment();

    // Dynamic pool tests
    test_dynamic_pool_basic();
    test_dynamic_pool_various_sizes();
    test_dynamic_pool_canary_corruption();
    test_dynamic_pool_alignment();
    test_dynamic_pool_coalesce();
    test_dynamic_pool_block_recovery();

    printf("All tests passed!\n");
    return 0;
}
