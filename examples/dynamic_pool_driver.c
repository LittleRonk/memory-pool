#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <dynamic_pool.h>
#include <pool_errors.h>

int main(void)
{
    // Create a pool
    PoolDyn *pool = pool_dyn_create(sizeof(int) * 10);
    if (pool)
        printf("Pool created\ncapacity: %zu\n", pool_dyn_capacity(pool));
    else
        fprintf(stderr, "Failed to create pool, returned error %d\n",
                pool_get_last_error());

    // Allocate memory for a variable of type int
    int *x = pool_dyn_alloc(pool, sizeof(int));
    if (x)
    {
        *x = 69;
        printf("x = %d\n", *x);
        printf("Pool size: %zu\n", pool_dyn_size(pool));
    }

    // Allocate memory for a variable of type float
    float *y = pool_dyn_alloc(pool, sizeof(float));
    if (y)
    {
        *y = -69.69;
        printf("y = %.2f\n", *y);
        printf("Pool size: %zu\n", pool_dyn_size(pool));
    }

    // We output the saved values of previously created variables
    printf("x: %d | y: %.2f\n", *x, *y);

    // Freeing the block
    POOL_CHECK_ERROR(pool_dyn_free(pool, x));

    // Destroying the pool
    POOL_CHECK_ERROR(pool_dyn_destroy(pool));

    printf("Testing completed.\n");

    return 0;
}
