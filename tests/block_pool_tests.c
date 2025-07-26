#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <pool_errors.h>
#include <block_pool.h>

void test_block_pool_basic(void) 
{
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

void test_block_pool_overflow(void) 
{
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

void test_block_pool_invalid_free(void) 
{
    PoolBlock *pool = pool_block_create(5, 16);
    assert(pool != NULL);

    // Attempt to free NULL
    pool_block_free(pool, NULL); // Should not crash
    assert(pool_last_error == POOL_NULL_PTR);

    // Attempt to free a pointer outside the pool
    int dummy;
    pool_block_free(pool, &dummy); // Should not crash
    assert(pool_last_error == POOL_INVALID_PTR);

    pool_block_destroy(pool);
    printf("test_block_pool_invalid_free: OK\n");
}

void test_block_pool_alignment(void)
{
    PoolBlock *pool = pool_block_create(5, 4);
    assert(pool != NULL);

    // Allocating blocks
    void *block_1 = pool_block_alloc(pool);
    assert(block_1 != NULL);

    void *block_2 = pool_block_alloc(pool);
    assert(block_2 != NULL);

    void *block_3 = pool_block_alloc(pool);
    assert(block_3 != NULL);

    void *block_4 = pool_block_alloc(pool);
    assert(block_4 != NULL);
    
    void *block_5 = pool_block_alloc(pool);
    assert(block_5 != NULL);

    // We check the alignment of each block.
    assert((uintptr_t) block_1 % BLOCK_POOL_ALIGNMENT == 0);
    assert((uintptr_t) block_2 % BLOCK_POOL_ALIGNMENT == 0);
    assert((uintptr_t) block_3 % BLOCK_POOL_ALIGNMENT == 0);
    assert((uintptr_t) block_4 % BLOCK_POOL_ALIGNMENT == 0);
    assert((uintptr_t) block_5 % BLOCK_POOL_ALIGNMENT == 0);

    // Freeing blicks
    pool_block_free(pool, block_1);
    assert(pool_last_error == POOL_OK);

    pool_block_free(pool, block_2);
    assert(pool_last_error == POOL_OK);

    pool_block_free(pool, block_3);
    assert(pool_last_error == POOL_OK);

    pool_block_free(pool, block_4);
    assert(pool_last_error == POOL_OK);

    pool_block_free(pool, block_5);
    assert(pool_last_error == POOL_OK);

    // Allocate memory again and check alignment
    block_1 = pool_block_alloc(pool);
    assert(block_1 != NULL);
    assert((uintptr_t) block_1 % BLOCK_POOL_ALIGNMENT == 0);

    pool_block_destroy(pool);
    printf("test_block_pool_alignment: OK\n");
}
