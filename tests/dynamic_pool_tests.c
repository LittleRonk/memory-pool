#include <stdio.h>
#include <assert.h>
#include <dynamic_pool.h>
#include <pool_errors.h>

void test_dynamic_pool_basic() {
    const size_t capacity = 1024; // 1 KB
    
    // Create a pool
    PoolDyn *pool = pool_dyn_create(capacity);
    assert(pool != NULL && pool_last_error == POOL_OK);

    // Allocate memory
    void *block1 = pool_dyn_alloc(pool, 64);
    assert(block1 != NULL && pool_last_error == POOL_OK);
    assert(pool_dyn_size(pool) > 0);

    // Freeing up memory
    pool_dyn_free(pool, block1);
    assert(pool_last_error == POOL_OK);

    // Clearing the pool
    pool_dyn_clear(pool);
    assert(pool->size == sizeof(MetaData) && "Pool should be reset");

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_basic: OK\n");
}

void test_dynamic_pool_various_sizes() {
    PoolDyn *pool = pool_dyn_create(2048);
    assert(pool != NULL);

    // Small block allocation
    void *small_block = pool_dyn_alloc(pool, 8);
    assert(small_block != NULL);

    // Large block allocation
    void *large_block = pool_dyn_alloc(pool, 512);
    assert(large_block != NULL);

    // Attempt to allocate too large block
    void *huge_block = pool_dyn_alloc(pool, 4096);
    assert(huge_block == NULL && pool_last_error == POOL_ALLOC_FAILED);

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_various_sizes: OK\n");
}

void test_dynamic_pool_canary_corruption() {
    PoolDyn *pool = pool_dyn_create(1024);
    assert(pool != NULL);

    void *block = pool_dyn_alloc(pool, 32);
    assert(block != NULL);

    // Damaging the canary
    MetaData *meta = (MetaData *)((byte *)block - sizeof(MetaData));
    meta->canary = 0xDEADBEEF; // Некорректное значение

    // Attempting to free a damaged block
    pool_dyn_free(pool, block);
    assert(pool_last_error == POOL_BLOCK_DAMAGED);

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_canary_corruption: OK\n");
}
