/**
 * @file: block_pool.h
 * @brief: Implementation of a memory pool based on a two-dimensional array.
 *
 * The memory pool stores fixed-size elements. The first byte of each
 * element is used as a busy flag.
 */

#ifndef BLOCK_POOL_H
#define BLOCK_POOL_H

#include <stddef.h>

/* Define a byte as an unsigned char, which is typically 8 bits and
 * represents a single byte of memory */
typedef unsigned char byte;

/* Defining the structure of a memory pool */
typedef struct pool_block {
    void *mem_pool;         // Pointer to the beginning of the allocated pool.
    size_t capacity;      // Maximum number of pool elements.
    size_t block_size;    // The size of one element in bytes.
    size_t size;          // The number of occupied pool blocks.
} PoolBlock;

/**
 * @brief: Creates a memmory pool.
 *
 * @param capacity: Memory pool size.
 * @param block_size: The size of one element in bytes.
 * @return: Pointer to the memory pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_INVALID_ARGS: Invalid arguments passed.
 *          -POOL_ALLOC_FAILED: Failed to allocate memory for pool.
 */
PoolBlock *pool_block_create(size_t capacity, size_t block_size);

/**
 * @brief: Requests memory from the pool.
 *
 * @param pool: Pointer to the memoty pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 *          -POOL_ALLOC_FAILED: Failed to allocate memory.
 */
void *pool_block_alloc(PoolBlock *pool);

/**
 * @brief: Frees previously allocated memory.
 *
 * @param pool: Pointer to the memory pool from which the
 * freed block was allocated.
 * @param memblock: Pointer to a block of memory allocated
 * from the pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
void pool_block_free(PoolBlock *pool, void *memblock);

/**
 * @brief: Frees all pool memory blocks.
 *
 * @param pool: Pointer to the memory pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
void pool_block_clear(PoolBlock *pool);

/**
 * @brief: Destroys the pool and frees the memory.
 *
 * The caller is responsible for the dangling pointer itself.
 *
 * @param pool: Pointer to the memory pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
void pool_block_destroy(PoolBlock *pool);

/**
 * @brief Returns the current size of the pool's occupied space.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
size_t pool_block_size(PoolBlock *pool);

/**
 * @brief Returns the total size of the pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
size_t pool_block_capacity(PoolBlock *pool);


#endif // BLOCK_POOL_H
