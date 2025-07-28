#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <dynamic_pool.h>
#include <pool_errors.h>

void test_dynamic_pool_basic()
{
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

void test_dynamic_pool_various_sizes()
{
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

void test_dynamic_pool_canary_corruption()
{
    PoolDyn *pool = pool_dyn_create(1024);
    assert(pool != NULL);

    void *block = pool_dyn_alloc(pool, 32);
    assert(block != NULL);

    // Damaging the canary
    MetaData *meta = (MetaData *)(block - sizeof(MetaData));
    meta->canary = 0xDEADBEEF; // Некорректное значение

    // Attempting to free a damaged block
    pool_dyn_free(pool, block);             // The block will be automatically restored
    assert(pool_last_error == POOL_OK);

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_canary_corruption: OK\n");
}

void test_dynamic_pool_alignment()
{
    PoolDyn *pool = pool_dyn_create(1024);
    assert(pool != NULL);

    // We check alignment of the pool.
    assert((uintptr_t) pool->mem_pool % ALIGNMENT == 0);

    // We check the alignment of the allocated block.
    void *block_1 = pool_dyn_alloc(pool, 64);
    assert(block_1 != NULL);
    assert((uintptr_t) block_1 % ALIGNMENT == 0);

    void *block_2 = pool_dyn_alloc(pool, 25);
    assert(block_2 != NULL);
    assert((uintptr_t) block_2 % ALIGNMENT == 0);

    void *block_3 = pool_dyn_alloc(pool, 15);
    assert(block_3 != NULL);
    assert((uintptr_t) block_3 % ALIGNMENT == 0);

    // Passing an invalid pointer
    pool_dyn_free(pool, block_1 - 1);
    assert(pool_last_error == POOL_INVALID_PTR);

    pool_dyn_free(pool, block_2 - 4);
    assert(pool_last_error == POOL_INVALID_PTR);

    pool_dyn_free(pool, block_3 + 1);
    assert(pool_last_error == POOL_INVALID_PTR);

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_alignment: OK\n");
}

void test_dynamic_pool_coalesce(void)
{
    PoolDyn *pool = pool_dyn_create(256);
    assert(pool != NULL);

    // Create many small sized blocks
    void *block_1 = pool_dyn_alloc(pool, 32);
    assert(block_1 != NULL);

    void *block_2 = pool_dyn_alloc(pool, 32);
    assert(block_2 != NULL);

    void *block_3 = pool_dyn_alloc(pool, 32);
    assert(block_3 != NULL);

    void *block_4 = pool_dyn_alloc(pool, 32);
    assert(block_4 != NULL);

    void *block_5 = pool_dyn_alloc(pool, 32);
    assert(block_5 != NULL);

    void *block_6 = pool_dyn_alloc(pool, 16);
    assert(block_6 != NULL);

    // We free 3 consecutive blocks.
    pool_dyn_free(pool, block_3);
    pool_dyn_free(pool, block_4);
    pool_dyn_free(pool, block_5);

    // Trying to allocate a large block.
    void *block_l = pool_dyn_alloc(pool, 128);
    assert(block_l == NULL);

    // We run the free block merging function and try to
    // allocate a large block again
    coalesce_free_blocks(pool);

    MetaData *block_3_meta = (MetaData *) (block_3 - sizeof(MetaData));
    block_l = pool_dyn_alloc(pool, 128);
    assert(block_l != NULL);

    // Freeing up 2 more blocks
    pool_dyn_free(pool, block_1);
    pool_dyn_free(pool, block_2);

    // The pool is fragmented, we can't allocate a large block
    void *block_ll = pool_dyn_alloc(pool, 64);
    assert(block_ll == NULL);

    // pool_dyn_alloc_safe function will try to automatically merge
    // free blocks and retry block allocation
    block_ll = pool_dyn_alloc_safe(pool, 64);
    assert(block_ll != NULL);

    pool_dyn_destroy(pool);
    assert(pool_last_error == POOL_OK);

    printf("test_dynamic_pool_coalesce: OK\n");
}

void test_dynamic_pool_block_recovery(void)
{
    PoolDyn *pool = pool_dyn_create(512);
    assert(pool != NULL);

    void *block_1 = pool_dyn_alloc(pool, 8);
    assert(block_1 != NULL);

    void *block_2 = pool_dyn_alloc(pool, 16);
    assert(block_2 != NULL);

    MetaData *block_2_meta = block_2 - sizeof(MetaData);

    typedef struct example {
        uint64_t one;
        uint64_t two;
    } exm;

    // We damage the block_2
    exm *ff = block_1;
    ff->one = 0xF123F;
    ff->two = 0xFFFFFFFFFFFFFFFF;

    // When the block is released, automatic recovery should occur
    pool_dyn_free(pool, block_2);
    assert(pool_last_error == POOL_OK);

    restore_block(pool, block_2);
    assert(pool_last_error == POOL_OK);

    restore_block(pool, NULL);
    assert(pool_last_error == POOL_NULL_PTR);

    restore_block(pool, block_1);
    assert(pool_last_error == POOL_OK);

    // We are passing incorrect pointers
    restore_block(pool, block_1 - 1);
    assert(pool_last_error == POOL_INVALID_PTR);

    restore_block(pool, block_1 - 8);
    assert(pool_last_error == POOL_INVALID_PTR);

    restore_block(pool, block_1 + 1);
    assert(pool_last_error == POOL_INVALID_PTR);

    restore_block(pool, block_1 + 8);
    assert(pool_last_error == POOL_INVALID_PTR);

    pool_dyn_destroy(pool);
    printf("test_dynamic_pool_block_recovery: OK\n");
}
