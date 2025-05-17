#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <dynamic_pool.h>
#include <pool_errors.h>

PoolDyn *pool_dyn_create(size_t capacity)
{
    pool_last_error = POOL_OK;
    PoolDyn *new_pool = calloc(1, sizeof(PoolDyn));
    if (!new_pool)
    {
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    /** 
     * We compensate for the memory occupied by metadata
     * by default, 25% more memory is allocated.
     * Then we align this value to the nearest multiple
     * of 24 (the minimum block size including metadata)
     */
    size_t final_capacity = capacity * ADVANCE;
    final_capacity = ALIGN_UP(final_capacity, sizeof(MetaData) + MIN_ALLOC_SIZE);

    byte advance_flag = 0; // Compensation flag

    /** 
     * Overflow check
     * We don't allocate more memory (in bytes) than the
     * maximum value of the size_t type.
     */
    void *mem_pool;
    if (final_capacity < capacity)
    {
        final_capacity = ALIGN_UP(capacity, sizeof(MetaData) + MIN_ALLOC_SIZE);
        if (final_capacity < capacity)
            final_capacity = ALIGN_DOWN(capacity, sizeof(MetaData) + MIN_ALLOC_SIZE);
        mem_pool = malloc(final_capacity);
    }
    else
    {
        mem_pool = malloc(final_capacity);
        advance_flag = 1;
    }

    if (!mem_pool)
    {
        pool_last_error = POOL_ALLOC_FAILED;
        free(new_pool);
        return NULL;
    }

    MetaData *block_meta = mem_pool;
    if (advance_flag)
    {
        block_meta->size = final_capacity - sizeof(MetaData);
        new_pool->capacity = final_capacity;
    }
    else
    {
        block_meta->size = capacity - sizeof(MetaData);
        new_pool->capacity = capacity;
    }

    block_meta->next_block = NULL;
    block_meta->canary = CANARY_FREE;
    new_pool->mem_pool = mem_pool;
    new_pool->size = sizeof(MetaData);

    return new_pool;
}

void *pool_dyn_alloc(PoolDyn *pool, size_t size)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return NULL;
    }

    /**
     * We align the requested amount of memory to the
     * nearest multiple of 8 (minimum amount of memory allocated
     * in bytes)
     */
    size_t align_size = ALIGN_UP(size, 8);

    if (align_size > (pool->capacity - pool->size))
    {
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    /**
     * We are looking for a free block of suitable size
     */
    MetaData *block = pool->mem_pool;
    bool find_flag = false;
    int i = 0;
    while (block)
    {
        if (block->canary == CANARY_FREE && block->size >= align_size)
        {
            find_flag = true;
            break;
        }

        /**
         * If the canary of even one block is corrypted, further use of the
         * pool may result in unpredictable behavior
         */
        if (block->canary != CANARY_FREE && block->canary != CANARY_USED)
            return NULL;

        block = block->next_block;
    }

    if (find_flag)
    {
        /**
         * If after allocating the required amount of memory in the block
         * there is enough space left for a new block of the minimum size,
         * we divide the original block into 2 blocks
         */
        if (block->size >= (sizeof(MetaData) + align_size + 8))
        {
            MetaData *new_block = (void *) block + sizeof(MetaData) + align_size;
            new_block->canary = CANARY_FREE;
            new_block->size = block->size - sizeof(MetaData) - align_size;
            new_block->next_block = block->next_block;

            block->size = align_size;
            block->next_block = new_block;
            pool->size += sizeof(MetaData) + align_size;
        }
        else
            pool->size += align_size;

        block->canary = CANARY_USED;
        return (void *) block + sizeof(MetaData);
    }

    pool_last_error = POOL_ALLOC_FAILED;
    return NULL;
}

void pool_dyn_free(PoolDyn *pool, void *block)
{
    pool_last_error = POOL_OK;
    if (!pool || !block)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    // We check that the transferred block address belong to the pool
    if (block < pool->mem_pool || block > pool->mem_pool + pool->capacity)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    // Check alignment
    unsigned int alignment = sizeof(MetaData) + MIN_ALLOC_SIZE;
    if (((uintptr_t) pool->mem_pool - (uintptr_t) block) % alignment != 0)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    MetaData *block_meta = block - sizeof(MetaData);
    // Checking the block's canary
    if (block_meta->canary != CANARY_FREE && block_meta->canary != CANARY_USED)
    {
        pool_last_error = POOL_BLOCK_DAMAGED;
        return;
    }
    block_meta->canary = CANARY_FREE;
    pool->size -= block_meta->size;

}

void pool_dyn_clear(PoolDyn *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    MetaData *block_meta = pool->mem_pool;
    pool->size = sizeof(MetaData);
    block_meta->size = pool->capacity - sizeof(MetaData);
    block_meta->canary = CANARY_FREE;
}

void pool_dyn_destroy(PoolDyn *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    free(pool->mem_pool);
    free(pool);
}

size_t pool_dyn_size(PoolDyn *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return 0;
    }
    return pool->size;
}

size_t pool_dyn_capacity(PoolDyn *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return 0;
    }
    return pool->capacity;
}
