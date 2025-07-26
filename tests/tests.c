#include <stdio.h>
#include "tests.h"

int main() {
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

    printf("All tests passed!\n");
    return 0;
}
