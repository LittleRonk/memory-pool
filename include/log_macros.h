/**
 * @file log_macros.h
 * @brief Macros for error logging
 */
#ifndef LOG_MACROS_H
#define LOG_MACROS_H

// ERROR LEVEL

// Macro for logging pool creation error
#define LOG_POOL_CREATE_ERROR(size) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[ERROR] Pool was not created, the system cannot allocate enough memory: %lu byte\n", (size));\
    logging(logger_buffer, LOG_LEVEL_ERROR); }\
    while (0)

// Macro for logging block allocation error (Not enough free space)
#define LOG_POOL_NOT_FREE_SPACE(free_memory, memory_required) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[ERROR] Allocation failed, there is not enough free memory in the pool.\n"\
            "Amount of free memory: %lu\nMemory required: %lu\n", (free_memory), (memory_required));\
    logging(logger_buffer, LOG_LEVEL_ERROR); }\
    while (0)

// Macro for logging block allocation error (the pool is highly fragmented)
#define LOG_POOL_FRAGMENTED(memory_required) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[ERROR] Allocation failed, the pool is highly fragmented.\nMemory required: %lu\n",\
            (memory_required));\
    logging(logger_buffer, LOG_LEVEL_ERROR); }\
    while (0)

// Macro for logging optimization error message
#define LOG_POOL_OPTIMIZE_ERROR(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[ERROR] Error trying to optimize pool %p\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_ERROR); }\
    while (0)

// Macro for logging block recovery error message
#define LOG_BLOCK_RECOVERY_FAILED(block) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[ERROR] Failed to restore block %p\n", (block));\
    logging(logger_buffer, LOG_LEVEL_ERROR); }\
    while (0)

// WARN LEVEL

// Macro for logging NULL pointer error
#define LOG_POOL_NULL_PTR do {\
    snprintf(logger_buffer, sizeof(logger_buffer), "[WARN] NULL pointer was passed. Func: %s\n", __func__);\
    logging(logger_buffer, LOG_LEVEL_WARN); }\
    while (0)

// Macro for logging pointer error (this pointer does not belong to the pool)
#define LOG_POOL_ALIEN_PTR do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[WARN] Someone else's pointer was passed. Func: %s\n", __func__);\
    logging(logger_buffer, LOG_LEVEL_WARN); }\
    while (0)

// Macro for logging pointer alignment error
#define LOG_POOL_PTR_NOT_ALIGNMENT do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[WARN] The passed pointer is not aligned. Func %s\n", __func__);\
    logging(logger_buffer, LOG_LEVEL_WARN); }\
    while (0)

// Macro for logging invalid pointer message (pointer is not the start of a block)
#define LOG_POOL_INVALID_PTR(ptr) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[WARN] Pointer %p is not the start of a block. Func %s\n", (ptr), __func__);\
    logging(logger_buffer, LOG_LEVEL_WARN); }\
    while (0)

// Macro for logging a message about a damaged block
#define LOG_BLOCK_DAMAGED(block) do {\
    snprintf(logger_buffer, sizeof(logger_buffer), "[WARN] Block %p is damaged\n", (block));\
    logging(logger_buffer, LOG_LEVEL_WARN); }\
    while (0)

// INFO LEVEL

// Macro for logging optimizarion failed message
#define LOG_POOL_OPTIMIZE_FAILED(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Pool %p optimization failed (no result)\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging optimization successful message
#define LOG_POOL_OPTIMIZE_SUCCESSFUL(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Pool %p optimization was successful (several blocks ware merged)\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging block recovery attempt
#define LOG_RESTORE_BLOCK(block) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Attempt to restore block %p\n", (block));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging a message about successful block recovery
#define LOG_BLOCK_SUCCESSFUL_RECOVERY(block) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Block %p successfully restored\n", (block));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging pool optimization attempt (partial fragmentation eliminator)
#define LOG_POOL_OPTIMIZATION_ATTEMPT(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Trying to optimize pool %p\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging pool creation information
#define LOG_POOL_CREATE_INFO(capacity, min_block_size, start_address) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Pool has been created: capacity - %lu | min block size - %d | start address - %p\n",\
            (capacity), (min_block_size), (start_address));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging message about pool cleanup
#define LOG_POOL_CLEANUP(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Pool %p cleanup\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging message about pool destruction
#define LOG_POOL_DESTROYED(pool) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Pool %p was destroyed\n", (pool));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// DEBUG LEVEL

// Macro for logging block allocation message
#define LOG_BLOCK_ALLOCATION(pool, block, block_size) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Allocated block: %p | Pool: %p | Block size: %lu\n", (block), (pool), (block_size));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

// Macro for logging message about block release
#define LOG_BLOCK_FREE(pool, block, block_size) do {\
    snprintf(logger_buffer, sizeof(logger_buffer),\
            "[INFO] Released block: %p | Pool: %p | Block size: %lu\n", (block), (pool), (block_size));\
    logging(logger_buffer, LOG_LEVEL_INFO); }\
    while (0)

#endif // LOG_MACROS_H
