#ifndef TESTS_H
#define TESTS_H

// Block pool tests
/**
 * @brief We check the correctness of pool creation/deletion.
 */
void test_block_pool_basic(void);

/**
 * @brief we check the correctness of allocation and release of
 * memory blocks.
 */
void test_block_pool_overflow(void);

/**
 * @brief Checking handling of overflow and invalid pointers.
 */
void test_block_pool_invalid_free(void);

// Dynamic pool tests
/**
 * @brief We check the operation of the main operations (allocation,
 * release, cleanup).
 */
void test_dynamic_pool_basic(void);

/**
 * @brief We check the work with different block size.
 */
void test_dynamic_pool_various_sizes(void);

/**
 * @brief Metadata corruption detection (canaries).
 */
void test_dynamic_pool_canary_corruption(void);

/**
 * @brief Testing pool and block alignment.
 */
void test_dynamic_pool_alignment(void);

/**
 * @brief Testing pool fregmentation
 */
void test_dynamic_pool_coalesce(void);

#endif // TESTS_H
