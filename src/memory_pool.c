#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory_pool.h>

Pool *pool_create(size_t capacity, size_t block_size)
{
    if ((capacity == 0) || (block_size == 0))
        return NULL;
    
    Pool *new_pool = calloc(1, sizeof(Pool));
    if (!new_pool)
        return NULL;

    /*
     * We allocate memory for the pool, add 1 byte to the block size
     * to store the busy flag
     */
    byte (*new_buffer)[block_size + 1] = calloc(capacity, block_size + 1);
    if (!new_buffer)
        goto buffer_allocation_error;

    new_pool->buffer = new_buffer;
    new_pool->capacity = capacity;
    new_pool->block_size = block_size + 1;
    new_pool->size = 0;

    return new_pool;


    buffer_allocation_error:
        free(new_pool);
    return NULL;
}

void *pool_alloc(Pool *pool)
{
    if (!pool || (pool->size == pool->capacity))
        return NULL;

    byte (*buffer)[pool->block_size] = pool->buffer;
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
bool memory_pool_contains(const Pool *pool, const void *memblock)
{
    // Check if the block is within the bounds of the pool's memory.
    void *pool_start = pool->buffer;
    void *pool_end = (byte *) pool_start + pool->capacity * pool->block_size;
    if (memblock < pool_start || memblock > pool_end)
        return false;

    // Check if the block is the start of block in the pool.
    byte *block_start = NULL;
    byte (*buffer)[pool->block_size] = pool->buffer;
    for (int i = 0; i < pool->capacity; ++i)
    {
        block_start = *(buffer + i);
        if (block_start == (byte *) memblock)
            return true;
    }

    return false;
}

void pool_free(Pool *pool, void *memblock)
{
    if (!pool || !memblock)
        return;

    /*
     * Calculate the address of the flag byte (the first byte of the block).
     * The user is given a pointer to the second byte of the block, so we
     * decrement it by 1.
     */
    byte *free_flag = (byte *) memblock - 1;

    /*
     * Check if the flag byte is within the pool's memory range and if it
     * is the start of a block.
     */
    if (!memory_pool_contains(pool, free_flag))
            return;

    *free_flag = 0;
    --pool->size;
}

void pool_clear(Pool *pool)
{
    if (!pool || (pool->size == 0))
        return;

    byte (*buffer)[pool->block_size] = pool->buffer;
    for (int i = 0; i < pool->size; ++i)
        **(buffer + i) = 0;

    pool->size = 0;
}

void pool_destroy(Pool *pool)
{
    if (!pool)
        return;
    free(pool->buffer);
    free(pool);
}
