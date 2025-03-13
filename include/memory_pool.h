#ifndef POOL_H
#define POOL_H

#include <stddef.h>
#include <stdbool.h>

/*
 * @file: pool.h
 * @brief: Implementation of a memory pool based on a two-dimensional array.
 *
 * The memory pool stores fixed-size elements. The first byte of each
 * element is used as a busy flag.
 */

/* Define a byte as an unsigned char, which is typically 8 bits and
 * represents a single byte of memory */
typedef unsigned char byte;

/* Defining the structure of a memory pool */
typedef struct pool {
    void *buffer;         // Pointer to the beginning of the allocated pool.
    size_t capacity;      // Maximum number of pool elements.
    size_t block_size;    // The size of one element in bytes.
    size_t size;          // The number of occupied pool blocks.
} Pool;

/*
 * @brief: Creates a memmory pool.
 *
 * @param capacity: Memory pool size.
 * @param block_size: The size of one element in bytes.
 * @return: Pointer to the memory pool.
 */
Pool *pool_create(size_t capacity, size_t block_size);

/*
 * @brief: Requests memory from the pool.
 *
 * @param pool: Pointer to the memoty pool.
 */
void *pool_alloc(Pool *pool);

/*
 * @brief: Frees previously allocated memory.
 *
 * @param pool: Pointer to the memory pool from which the
 * freed block was allocated.
 * @param memblock: Pointer to a block of memory allocated
 * from the pool.
 */
void pool_free(Pool *pool, void *memblock);

/*
 * @brief: Frees all pool memory blocks.
 *
 * @param pool: Pointer to the memory pool.
 */
void pool_clear(Pool *pool);

/*
 * @brief: Destroys the pool and frees the memory.
 *
 * The caller is responsible for the dangling pointer itself.
 *
 * @param pool: Pointer to the memory pool.
 */
void pool_destroy(Pool *pool);

#endif // POOL_H
