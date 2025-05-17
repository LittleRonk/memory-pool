#include <stdio.h>
#include <stdlib.h>
#include <block_pool.h>

// Example data structure that we will store in the pool
typedef struct {
    int id;
    char name[32];
} Data;

int main() {
    // Create a pool with capacity = 5 and block_size = sizeof(Data)
    size_t capacity = 5;
    size_t block_size = sizeof(Data);
    PoolBlock *pool = pool_block_create(capacity, block_size);

    if (!pool) {
        printf("Error creating the pool!\n");
        return 1;
    }

    printf("PoolFixed successfully created. Capacity: %zu, Block size: %zu\n", pool->capacity, pool->block_size);

    // Allocate several memory blocks from the pool
    Data *data1 = (Data *)pool_block_alloc(pool);
    Data *data2 = (Data *)pool_block_alloc(pool);
    Data *data3 = (Data *)pool_block_alloc(pool);

    if (!data1 || !data2 || !data3) {
        printf("Error allocating memory from the pool!\n");
        pool_block_destroy(pool);
        return 1;
    }

    printf("Allocated 3 memory blocks from the pool.\n");

    // Fill data in the allocated blocks
    data1->id = 1;
    snprintf(data1->name, sizeof(data1->name), "Data 1");

    data2->id = 2;
    snprintf(data2->name, sizeof(data2->name), "Data 2");

    data3->id = 3;
    snprintf(data3->name, sizeof(data3->name), "Data 3");

    // Print the data
    printf("Data 1: id = %d, name = %s\n", data1->id, data1->name);
    printf("Data 2: id = %d, name = %s\n", data2->id, data2->name);
    printf("Data 3: id = %d, name = %s\n", data3->id, data3->name);

    // Free one of the memory blocks
    printf("Freeing block data2...\n");
    pool_block_free(pool, data2);

    // Try to allocate another block (should reuse the freed block)
    Data *data4 = (Data *)pool_block_alloc(pool);
    if (!data4) {
        printf("Error allocating memory from the pool!\n");
        pool_block_destroy(pool);
        return 1;
    }

    data4->id = 4;
    snprintf(data4->name, sizeof(data4->name), "Data 4");

    printf("Data 4: id = %d, name = %s\n", data4->id, data4->name);

    // Clear the entire pool
    printf("Clearing the pool...\n");
    pool_block_clear(pool);

    // Try to allocate another block after clearing
    Data *data5 = (Data *)pool_block_alloc(pool);
    if (!data5) {
        printf("Error allocating memory from the pool after clearing!\n");
        pool_block_destroy(pool);
        return 1;
    }

    data5->id = 5;
    snprintf(data5->name, sizeof(data5->name), "Data 5");

    printf("Data 5: id = %d, name = %s\n", data5->id, data5->name);

    // Destroy the pool
    printf("Destroying the pool...\n");
    pool_block_destroy(pool);

    printf("Testing completed.\n");
    return 0;
}
