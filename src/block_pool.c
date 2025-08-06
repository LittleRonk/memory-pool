#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pool_errors.h>
#include <pool_logger.h>
#include <log_macros.h>
#include <block_pool.h>

extern char logger_buffer[256];

PoolBlock *pool_block_create(size_t capacity, size_t block_size)
{
    pool_last_error = POOL_OK;
    if ((capacity == 0) || (block_size == 0))
    {
        LOG_POOL_INVALID_ARGS;
        pool_last_error = POOL_INVALID_ARGS;
        return NULL;
    }
    
    PoolBlock *new_pool = calloc(1, sizeof(PoolBlock));
    if (!new_pool)
    {
        LOG_POOL_CREATE_ERROR(sizeof(PoolBlock));
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    /**
     * The block size must be a multiple of the alignment.
     * We add the alignment value to be able to offset the payload
     * within the block.
     */
    size_t mult_block_size = MULTIPLE_UP(block_size, BLOCK_POOL_ALIGNMENT) +
        BLOCK_POOL_ALIGNMENT;

    byte (*mem_pool)[mult_block_size] = 
        calloc(capacity, mult_block_size);
    if (!mem_pool)
        goto buffer_allocation_error;

    new_pool->mem_pool = mem_pool;
    new_pool->capacity = capacity;
    new_pool->block_size = mult_block_size;
    new_pool->size = 0;
    new_pool->last_clear = NULL;
    new_pool->offset = MULTIPLE_UP((uintptr_t) mem_pool, BLOCK_POOL_ALIGNMENT) -
        (uintptr_t) mem_pool;

    return new_pool;


    buffer_allocation_error:
        LOG_POOL_CREATE_ERROR(sizeof(PoolBlock) + (capacity * block_size));
        pool_last_error = POOL_ALLOC_FAILED;
        free(new_pool);
    return NULL;
}

void *pool_block_alloc(PoolBlock *pool)
{
    pool_last_error = POOL_OK;

    if (!pool)
    {
        LOG_POOL_NULL_PTR;
        pool_last_error = POOL_NULL_PTR;
        return NULL;
    }

    if (pool->size == pool->capacity)
    {
        LOG_POOL_NOT_FREE_SPACE(pool->mem_pool, pool->capacity - pool->size, pool->block_size);
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    byte (*buffer)[pool->block_size] = pool->mem_pool;
    byte *free_flag = NULL;

    // Checking the last freed block
    if (pool->last_clear)
    {
        free_flag = pool->last_clear;
        if (*free_flag == 0)
        {
            *free_flag = 1;
            ++pool->size;
            pool->last_clear = NULL;
            LOG_BLOCK_ALLOCATION(pool->mem_pool, (void *) (free_flag + pool->offset), pool->block_size);
            return (void *) (free_flag + pool->offset);
        }
    }

    /*
     * We loop through the blocks and check the busy flags (first byte), if the 
     * block is free then we return the pointer to the second byte of the block
     */

    free_flag = pool->mem_pool;

    // Looking for a free block
    for (int i = 0; i < pool->capacity; ++i)
    {
        if (*free_flag == 0)
        {
            *free_flag = 1;
            ++pool->size;
            LOG_BLOCK_ALLOCATION(pool->mem_pool, (void *) (free_flag + pool->offset), pool->block_size);
            return (void *) (free_flag + pool->offset);
        }

        free_flag += pool->block_size;
    }

    pool_last_error = POOL_ALLOC_FAILED;
    return NULL;
}

/*
 * @brief: Checking if a block is included in the memory pool.
 *
 * @param pool: Pointer to the memory pool.
 * @param memblock: Pointer to a memory block.
 * @return: 'true' if the memory block is in the pool and corectly aligned,
 * otherwise 'false'.
 */
bool pool_block_contains(const PoolBlock *pool, const void *memblock)
{
    // Check if the block is within the bounds of the pool's memory.
    void *pool_start = pool->mem_pool;
    void *pool_end = pool_start + (pool->capacity * pool->block_size);
    if (memblock < pool_start || memblock > pool_end)
    {
        LOG_POOL_ALIEN_PTR(memblock);
        return false;
    }

    // Check if the block is the start of block in the pool.
    if (((uintptr_t) memblock - (uintptr_t)pool_start) %
        pool->block_size == 0)
        return true;
    else
    {
        LOG_POOL_PTR_NOT_ALIGNMENT(memblock);
    }
}

void pool_block_free(PoolBlock *pool, void *memblock)
{
    pool_last_error = POOL_OK;
    if (!pool || !memblock)
    {
        LOG_POOL_NULL_PTR;
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    /**
     * Calculate the address of the flag byte (the first byte of the block).
     * The user is given a pointer to the second byte of the block, so we
     * decrement it by 1.
     */
    byte *free_flag = (byte *) memblock - pool->offset;

    /**
     * Check if the flag byte is within the pool's memory range and if it
     * is the start of a block.
     */
    if (!pool_block_contains(pool, memblock))
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    *free_flag = 0;
    pool->last_clear = free_flag;
    --pool->size;
    LOG_BLOCK_FREE(pool->mem_pool, memblock, pool->block_size);
}

void pool_block_clear(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        LOG_POOL_NULL_PTR;
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    if (pool->size == 0)
        return;

    byte (*buffer)[pool->block_size] = pool->mem_pool;
    for (int i = 0; i < pool->size; ++i)
        **(buffer + i) = 0;

    pool->size = 0;
    LOG_POOL_CLEANUP(pool->mem_pool, pool->capacity);
}

void pool_block_destroy(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    free(pool->mem_pool);
    free(pool);
    LOG_POOL_DESTROYED(pool->mem_pool);
}

size_t pool_block_size(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        LOG_POOL_NULL_PTR;
        pool_last_error = POOL_NULL_PTR;
        return 0;
    }

    return pool->size;
}

size_t pool_block_capacity(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        LOG_POOL_NULL_PTR;
        pool_last_error = POOL_NULL_PTR;
        return 0;
    }

    return pool->capacity;
}
