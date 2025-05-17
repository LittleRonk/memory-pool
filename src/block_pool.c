#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pool_errors.h>
#include <block_pool.h>

PoolBlock *pool_block_create(size_t capacity, size_t block_size)
{
    pool_last_error = POOL_OK;
    if ((capacity == 0) || (block_size == 0))
    {
        pool_last_error = POOL_INVALID_ARGS;
        return NULL;
    }
    
    PoolBlock *new_pool = calloc(1, sizeof(PoolBlock));
    if (!new_pool)
    {
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    /*
     * We allocate memory for the pool, add 1 byte to the block size
     * to store the busy flag
     */
    byte (*mem_pool)[block_size + 1] = calloc(capacity, block_size + 1);
    if (!mem_pool)
        goto buffer_allocation_error;

    new_pool->mem_pool = mem_pool;
    new_pool->capacity = capacity;
    new_pool->block_size = block_size + 1;
    new_pool->size = 0;

    return new_pool;


    buffer_allocation_error:
        pool_last_error = POOL_ALLOC_FAILED;
        free(new_pool);
    return NULL;
}

void *pool_block_alloc(PoolBlock *pool)
{
    pool_last_error = POOL_OK;

    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return NULL;
    }

    if (pool->size == pool->capacity)
    {
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    byte (*buffer)[pool->block_size] = pool->mem_pool;
    byte *free_flag = NULL;

    /*
     * We loop through the blocks and check the busy flags (first byte), if the 
     * block is free then we return the pointer to the second byte of the block
     */
    for (int i = 0; i < pool->capacity; ++i)
    {
        free_flag = *(buffer + i);
        if (*free_flag == 0)
        {
            *free_flag = 1;
            ++pool->size;
            return (void *) (free_flag + 1);
        }
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
    void *pool_end = (byte *) pool_start + pool->capacity * pool->block_size;
    if (memblock < pool_start || memblock > pool_end)
        return false;

    // Check if the block is the start of block in the pool.
    byte *block_start = NULL;
    byte (*buffer)[pool->block_size] = pool->mem_pool;
    for (int i = 0; i < pool->capacity; ++i)
    {
        block_start = *(buffer + i);
        if (block_start == (byte *) memblock)
            return true;
    }

    return false;
}

void pool_block_free(PoolBlock *pool, void *memblock)
{
    pool_last_error = POOL_OK;
    if (!pool || !memblock)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    /**
     * Calculate the address of the flag byte (the first byte of the block).
     * The user is given a pointer to the second byte of the block, so we
     * decrement it by 1.
     */
    byte *free_flag = (byte *) memblock - 1;

    /**
     * Check if the flag byte is within the pool's memory range and if it
     * is the start of a block.
     */
    if (!pool_block_contains(pool, free_flag))
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    *free_flag = 0;
    --pool->size;
}

void pool_block_clear(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    if (pool->size == 0)
        return;

    byte (*buffer)[pool->block_size] = pool->mem_pool;
    for (int i = 0; i < pool->size; ++i)
        **(buffer + i) = 0;

    pool->size = 0;
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
}

size_t pool_block_size(PoolBlock *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
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
        pool_last_error = POOL_NULL_PTR;
        return 0;
    }

    return pool->capacity;
}
