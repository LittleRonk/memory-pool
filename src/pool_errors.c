#include <pool_errors.h>

PoolError pool_last_error = POOL_OK;

char *str_errors[] = {
    "POOL_OK",
    "POOL_NULL_PTR",
    "POOL_INVALID_PTR",
    "POOL_ALLOC_FAILED",
    "POOL_BLOCK_DAMAGED"
};

PoolError pool_get_last_error(void)
{
    return pool_last_error;
}
