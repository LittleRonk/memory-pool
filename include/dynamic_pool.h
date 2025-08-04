/**
 * @file: dynamic_pool.h
 * @brief: Implementing a memory pool with dynamic block size.
 *
 * The canary and other meta-information are placed at the beginning of each block.
 * The minimum memory size allocated is 8 bytes.
 */

#ifndef DYNAMIC_POOL
#define DYNAMIC_POOL

#include <stddef.h>
#include <stdint.h>

// The ftirst canary of the block
#define CANARY_FREE 0xFFFEC0DE
#define CANARY_USED 0xFFFFC0DE

// The second canary of the block
#define END_CANARY 0xC0DE5005E695005E

// Macros for alignment to the nearest multiple
#define MULTIPLICITY_UP(value, multiple) (((value) + (multiple - 1)) & ~((multiple) - 1))
#define MULTIPLICITY_DOWN(value, multiple) (value) & ~((multiple) - 1)

/**
 * Used to increase allocated memory to compensate
 * for metadata
 */
#define ADVANCE 1.3

/**
 * The minimum amount of memory that can be allocated.
 * Must be a power of TWO and NOT less 8.
 */
#define MIN_ALLOC_SIZE 8

// Used to align the pool address. It is forbidden to change this value.
#define ALIGNMENT 8

/**
 * Represents Meta information about a block
 */
# pragma pack(push, 1)              // Set alignment to 1
typedef struct meta_data {
    uint32_t canary;                // Serves as a block busy flag and metadata integrity flag
    unsigned int size;              // Block size
    struct meta_data *next_block;   // Points to the next block
    uint64_t end_canary;            // Necessary for correct recognition of block metadata signatyre
}MetaData;
# pragma pack(pop)

/**
 * Memory pool structure
 */
typedef struct pool_dyn {
    void *raw;           // Start of unaligned pool
    void *mem_pool;     // Start of aligned pool
    size_t capacity;    // Total size of the memory pool (in bytes)
    size_t size;        // Amount of allocated memory (in bytes)
} PoolDyn;

/**
 * @brief Creates a dynamic memory pool.
 *
 * @param capacity Size of the memory pool (in bytes).
 * @return Pointer tot the structure of the created memory pool,
 * or NULL if an error occurred.
 *
 * @errors:
 *      -POOL_OK: Function worked without errors.
 *      -POOL_ALLOC_FAILED: Failed to allocate memory for pool.
 */
PoolDyn *pool_dyn_create(size_t capacity);

/**
 *  @brief Allocates the requested amount of memory from the pool.
 *
 *  @param pool Pointer to the memory pool.
 *  @param size Amount of memory required.
 *  @return Pointer to the beginning of the allocated memory,
 *  NULL if an error occurred.
 *
 *  @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: Pool pointer is NULL.
 *          -POOL_ALLOC_FAILED: Failed to allocate the requested amount memory.
 *          -POOL_BLOCK_DAMAGED: One of the blocks is damaged.
 */
void *pool_dyn_alloc(PoolDyn *pool, size_t size);

/**
 * @brief pool_alloc function wrapper.
 *
 * If the allocation fails the function may attempt 
 * to merge free blocks and retry the allocation.
 *
 *  @param pool Pointer to the memory pool.
 *  @param size Amount of memory required.
 *  @return Pointer to the beginning of the allocated memory,
 *  NULL if an error occurred.
 *
 *  @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: Pool pointer is NULL.
 *          -POOL_ALLOC_FAILED: Failed to allocate the requested amount memory.
 *          -POOL_BLOCK_DAMAGED: One of the blocks is damaged.
 */
void *pool_dyn_alloc_safe(PoolDyn *pool, size_t size);


/**
 * @brief Frees previously allocated memory.
 *
 * @param pool Pointer to the pool from which the memory was allocated.
 * @param block Pointer to the memory to be freed.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool or block pointer is NULL.
 *          -POOL_INVALID_PTR: block pointer is not in the pool or is not aligned.
 *          -POOL_BLOCK_DAMAGED: One of the blocks is damaged.
 */
void pool_dyn_free(PoolDyn *pool, void *block);

/**
 * @brief Clears the pool.
 *
 * After using this function, pointers to all previously allocated
 * memory will become dangling.
 *
 * @param pool Pointer to the pool to be cleared.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTTR: pool pointer is NULL.
 */
void pool_dyn_clear(PoolDyn *pool);

/**
 * @brief Deletes a memory pool and frees all memory it occupied.
 *
 * The caller is responsible for the dangling pointer.
 *
 * @param pool Pointer to the pool to be destroyed.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
void pool_dyn_destroy(PoolDyn *pool);

/**
 * @brief Returns the current size of the pool's occupied space.
 * @param pool Pointer to the pool.
 * @return Size of occupied space.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
size_t pool_dyn_size(PoolDyn *pool);

/**
 * @brief Returns the total size of the pool.
 * @param pool Pointer to the pool.
 * @return Pool capacity.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
size_t pool_dyn_capacity(PoolDyn *pool);

/**
 * @brief Eliminates pool fragmentation by merging free blocks.
 * @param pool Pointer to the pool.
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool pointer is NULL.
 */
void coalesce_free_blocks(PoolDyn *pool);

/**
 * @brief Restoring damaged blocks
 * @param pool Pointer to the pool
 * @param block Pointer to the damaged block
 *
 * @errors:
 *          -POOL_OK: Function worked without errors.
 *          -POOL_NULL_PTR: pool or block pointer is NULL.
 *          -POOL_INVALID_PTR: The block pointer is not correct.
 */
void restore_block(PoolDyn *pool, void *block);

#endif // DYNAMIC_POOL
