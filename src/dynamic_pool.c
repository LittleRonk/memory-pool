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
     * by default, 30% more memory is allocated.
     */
    size_t final_capacity = capacity * ADVANCE;

    /** 
     * Overflow check
     * We don't allocate more memory (in bytes) than the
     * maximum value of the size_t type.
     */
    if (final_capacity < capacity)
        final_capacity = capacity;

    void *raw = malloc(final_capacity);
    if (!raw)
    {
        pool_last_error = POOL_ALLOC_FAILED;
        free(new_pool);
        return NULL;
    }

    // Align the address of the beginning of the pool
    void *mem_pool = (void *) (((uintptr_t) raw + (ALIGNMENT - 1)) &
            ~(ALIGNMENT - 1));

    MetaData *block_meta = mem_pool;

    // Empty pool is one big block
    block_meta->size = final_capacity - sizeof(MetaData);

    // If the pool been offst to align, the pool size is reduced by the offset difference
    new_pool->capacity = final_capacity - ((uintptr_t) raw - (uintptr_t) mem_pool);

    block_meta->next_block = NULL;
    block_meta->canary = CANARY_FREE;
    block_meta->end_canary = END_CANARY;
    new_pool->raw = raw;
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
     * Increase the allocated memory volume to the minimum
     * or round up to a multiple.
     */
    size_t alloc_size;
    if (size < MIN_ALLOC_SIZE)
        alloc_size = MIN_ALLOC_SIZE;
    else
        alloc_size = MULTIPLICITY_UP(size, MULTIPLICITY);

    if (alloc_size > (pool->capacity - pool->size))
    {
        pool_last_error = POOL_ALLOC_FAILED;
        return NULL;
    }

    /**
     * We are looking for a free block of suitable size
     */
    MetaData *block = pool->mem_pool;
    bool find_flag = false;
    while (block)
    {
        if (block->canary == CANARY_FREE && block->size >= alloc_size)
        {
            find_flag = true;
            break;
        }

        /**
         * If the canary of even one block is corrypted, further use of the
         * pool may result in unpredictable behavior
         */
        if (block->canary != CANARY_FREE && block->canary != CANARY_USED)
        {
            pool_last_error = POOL_BLOCK_DAMAGED;
            return NULL;
        }

        block = block->next_block;
    }

    if (find_flag)
    {
        /**
         * If after allocating the required amount of memory in the block
         * there is enough space left for a new block of the minimum size,
         * we divide the original block into 2 blocks
         */
        if (block->size >= (sizeof(MetaData) + alloc_size + MIN_ALLOC_SIZE))
        {
            MetaData *new_block = (void *) block + sizeof(MetaData) + alloc_size;
            new_block->canary = CANARY_FREE;
            new_block->size = block->size - sizeof(MetaData) - alloc_size;
            new_block->next_block = block->next_block;
            new_block->end_canary = END_CANARY;

            block->size = alloc_size;
            block->next_block = new_block;
            pool->size += sizeof(MetaData) + alloc_size;
        }
        else
            pool->size += alloc_size;

        block->canary = CANARY_USED;
        return (void *) block + sizeof(MetaData);
    }

    pool_last_error = POOL_ALLOC_FAILED;
    return NULL;
}

void *pool_dyn_alloc_safe(PoolDyn *pool, size_t size)
{
    void *block = pool_dyn_alloc(pool, size);
    
    /**
     * If the allocation fails, we check if there is
     * enough free space in the pool,
     * try to merge the free blocks, and retry the allocation.
     */
    if (!block && pool->capacity - pool->size >= size)
    {
        coalesce_free_blocks(pool);
        block = pool_dyn_alloc(pool, size);
    }

    return block;
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
    if ((uintptr_t) block % ALIGNMENT != 0)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    MetaData *block_meta = block - sizeof(MetaData);
    // Checking the block's canary
    if (block_meta->canary != CANARY_FREE && block_meta->canary != CANARY_USED)
        restore_block(pool, block);

    // If the block was not restored, return control
    if (pool_last_error != POOL_OK)
        return;

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

    free(pool->raw);
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

void coalesce_free_blocks(PoolDyn *pool)
{
    pool_last_error = POOL_OK;
    if (!pool)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    MetaData *block_1 = pool->mem_pool;
    MetaData *block_2 = block_1->next_block;

    while (block_2)
    {
        // If two adjacent blocks are free, we merge them.
        if (block_1->canary == CANARY_FREE &&
                block_2->canary == CANARY_FREE)
        {
            block_1->next_block = block_2->next_block;
            block_1->size += sizeof(MetaData) + block_2->size;
            pool->size -= sizeof(MetaData);

            /**
             * We move to the next block and try again (in case
             * free blocks follow each other)
             */
            block_2 = block_2->next_block;
            continue;
        }
        block_1 = block_2;
        block_2 = block_2->next_block;
    }
}

void restore_block(PoolDyn *pool, void *block)
{
    pool_last_error = POOL_OK;

    if (!pool || ! block)
    {
        pool_last_error = POOL_NULL_PTR;
        return;
    }

    // Check if the transferred address is in the range of the pool addresses
    if (block < pool->mem_pool || block > (pool->mem_pool + pool->capacity))
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    // If the alignment is incorrect, then the block address is incorrect (there is an offset)
    if ((uintptr_t) block % ALIGNMENT != 0)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    // If this condition is met, it means that an invalid pointer was passed
    if (block - sizeof(MetaData) < pool->mem_pool)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }

    // Metadata of the block to be restored
    MetaData *block_meta = block - sizeof(MetaData);


    // Checking the canaries
    if (block_meta->canary == CANARY_FREE || block_meta->canary == CANARY_USED)
        // If the first canary is not damaged, then the block is not damaged
        return;

    MetaData *previous_block = NULL;   // Previous block metadata
    MetaData *next_block = NULL;       // Next block metadata

    uint64_t *end_canary = block;  // To track the second canary

    while(pool->mem_pool != (void *) end_canary)
    {
        if (*end_canary == END_CANARY)
        {
            /**
             * If a secind canary is found, we also check the first one to exclude
             * the possibility of a simple coincidence
             */
            previous_block = ((void *) (end_canary + 1)) - sizeof(MetaData);  // !!!
            if (previous_block->canary == CANARY_FREE || previous_block->canary == CANARY_USED)
                break;
            else
                previous_block = NULL;
        }

        --end_canary;
    }

    // To determine whether the damaged block is the last one
    void *pool_last_8_byte = pool->mem_pool + pool->capacity - 8; 
    end_canary = block;

    end_canary = block;
    /**
     * We track the address of the last 8 bytes so as not to go beyond
     * the pool if the transferred block turns out to be the last one
     */
    while (pool_last_8_byte != (void *) end_canary)
    {
        if (*end_canary == END_CANARY)
        {
            next_block = ((void *) (end_canary + 1)) - sizeof(MetaData);
            /**
             * If a secind canary is found, we also check the first one to exclude
             * the possibility of a simple coincidence
             */
            if (next_block->canary == CANARY_FREE || next_block->canary == CANARY_USED)
                break;
        }
        ++end_canary;
    }

    /**
     * The next_block pointer of the previous block must point to the block being
     * restored, if this is not the case then the passed pointer was incorrect.
     * If previous_block is not found, then we are working with the first block.
     */
    if (previous_block && previous_block->next_block != block_meta)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }
    else if (!previous_block && (void *) block_meta != pool->mem_pool)
    {
        pool_last_error = POOL_INVALID_PTR;
        return;
    }


    block_meta->canary = CANARY_USED;
    block_meta->next_block = next_block;
    block_meta->end_canary = END_CANARY;

    if (next_block)
        block_meta->size = (uintptr_t) next_block - (uintptr_t) block_meta - sizeof(MetaData);
    else
        block_meta->size = pool->capacity -
            ((uintptr_t) block_meta - (uintptr_t) pool->mem_pool) - sizeof(MetaData);
}
