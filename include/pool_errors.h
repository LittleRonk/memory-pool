/**
 * @file pool_errors.h
 * @brief Error codes for pool operations
 */

#ifndef POOL_ERRORS_H
#define POOL_ERRORS_H

// Error codes
typedef enum {
    POOL_OK = 0,        // Successful completion
    POOL_NULL_PTR,      // NULL pointer passed
    POOL_INVALID_PTR,   // Invalid pointer passed
    POOL_INVALID_ARGS,  // Invalig argument passed
    POOL_ALLOC_FAILED,  // Memory allocation error
    POOL_BLOCK_DAMAGED, // One of the blocks is damaged
} PoolError;

extern PoolError pool_last_error;

extern char *str_errors[];

// Function to get the last error
PoolError pool_get_last_error(void);

// Macro to check functions for errors
#define POOL_CHECK_ERROR(func_call) \
    do { \
        func_call; \
        PoolError last_error = pool_get_last_error(); \
        if (last_error != POOL_OK) { \
            fprintf(stderr, "Error in " #func_call": %s\n", str_errors[last_error]); \
            return 0; \
        } \
    } while (0)

#endif // POOL_ERRORS_H
