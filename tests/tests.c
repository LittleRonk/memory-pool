#include <stdio.h>
#include "tests.h"

int main() {
    // Block pool tests
    test_block_pool_basic();
    test_block_pool_overflow();
    test_block_pool_invalid_free();

    // Dynamic pool tests
    test_dynamic_pool_basic();
    test_dynamic_pool_various_sizes();
    test_dynamic_pool_canary_corruption();

    printf("All tests passed!\n");
    return 0;
}
