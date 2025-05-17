#include <assert.h>
#include <stdio.h>
#include <block_pool.h>

void test_block_pool_basic() {
    const size_t capacity = 10;
    const size_t block_size = 32;
    
    // Create a pool
    PoolBlock *pool = pool_block_create(capacity, block_size);
    assert(pool != NULL && "Pool creation failed");
    assert(pool->size == 0 && "Pool should be empty after creation");

    // Allocate a block of memory
    void *block1 = pool_block_alloc(pool);
    assert(block1 != NULL && "Allocation failed");
    assert(pool->size == 1 && "Pool size should increase after alloc");

    // Freeing the block
    pool_block_free(pool, block1);
    assert(pool->size == 0 && "Pool size should decrease after free");

    // Destroy the pool
    pool_block_destroy(pool);
    printf("test_block_pool_basic: OK\n");
}

void test_block_pool_overflow() {
    const size_t capacity = 2;
    const size_t block_size = 8;
    
    PoolBlock *pool = pool_block_create(capacity, block_size);
    assert(pool != NULL);

    // Allocate all blocks
    void *block1 = pool_block_alloc(pool);
    void *block2 = pool_block_alloc(pool);
    assert(block1 != NULL && block2 != NULL);
    assert(pool->size == capacity && "Pool should be full");

    // Attempt to allocate when all blocks are occuped
    void *block3 = pool_block_alloc(pool);
    assert(block3 == NULL && "Should return NULL when pool is full");

    pool_block_destroy(pool);
    printf("test_block_pool_overflow: OK\n");
}

void test_block_pool_invalid_free() {
    PoolBlock *pool = pool_block_create(5, 16);
    assert(pool != NULL);

    // Attempt to free NULL
    pool_block_free(pool, NULL); // Should not crash

    // Attempt to free a pointer outside the pool
    int dummy;
    pool_block_free(pool, &dummy); // Should not crash

    pool_block_destroy(pool);
    printf("test_block_pool_invalid_free: OK\n");
}
