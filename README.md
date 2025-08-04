# Memory Pool Implementation in C

Implementing pools with fixed and dynamic block sizes in C

## In Development

- **Logging**: Logging for a pool with a fixed block size

## Key Features

### General
- **Efficient allocation**: Memory is pre-allocated, so allocation is fast.
- **Manual memory management**: You control when memory is freed or cleared.

### Block Pool
- **Fixed-size memory blocks**: All blocks in the pool have the same size.

### Dynamic Pool
- **Dynamically changing block size**: Block size depends on the amount of memory requested

## Usage

### Building the Project

To build the project, use the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

### Running the Example

After building the project, you can run the example driver program:

```bash
cd examples
./block_pool_driver
```
or
```bash
cd examples
./dynamic_pool_driver
```

### Example Code

Here is an example of how to use the memory pool with fixed block size:

```c
#include <stdio.h>
#include <stdlib.h>
#include <block_pool.h>

// Example data structure to store in the pool
typedef struct {
    int id;
    char name[32];
} Data;

int main() {
    // Create a memory pool with capacity for 5 blocks of size sizeof(Data)
    size_t capacity = 5;
    size_t block_size = sizeof(Data);
    Pool *pool = pool_block_create(capacity, block_size);

    if (!pool) {
        printf("Error: Failed to create the memory pool.\n");
        return 1;
    }

    printf("Memory pool created successfully. Capacity: %zu, Block size: %zu\n", pool->capacity, pool->block_size);

    // Allocate a block of memory from the pool
    Data *data = (Data *)pool_block_alloc(pool);
    if (!data) {
        printf("Error: Failed to allocate memory from the pool.\n");
        pool_block_destroy(pool);
        return 1;
    }

    // Use the allocated memory
    data->id = 1;
    snprintf(data->name, sizeof(data->name), "Example Data");

    printf("Data allocated: id = %d, name = %s\n", data->id, data->name);

    // Free the memory block
    printf("Freeing the memory block...\n");
    pool_block_free(pool, data);

    // Clear the entire pool (optional)
    printf("Clearing the pool...\n");
    pool_block_clear(pool);

    // Destroy the pool and free all memory
    printf("Destroying the pool...\n");
    pool_block_destroy(pool);

    printf("Memory pool example completed.\n");
    return 0;
}
```
## API Overview

### Pool with fixed block size

- **PoolBlock \*pool_block_create(size_t capacity, size_t block_size)**: Creates a new memory pool.

- **void \*pool_block_alloc(PoolBlock \*pool)**: Allocates a block of memory from the pool.

- **void pool_block_free(PoolBlock \*pool, void \*memblock)**: Frees a previously allocated block.

- **void pool_block_clear(PoolBlock \*pool)**: Frees all blocks in the pool.

- **void pool_block_destroy(PoolBlock \*pool)**: Destroys the pool and frees all associated memory.

- **size_t pool_block_size(PoolBlock \*pool)**: Returns the current size of the pool's occupied space.

- **size_t pool_block_capacity(PoolBlock \*pool)**: Returns the total size of the pool.

### Dynamic pool

- **PoolDyn \*pool_dyn_create(size_t capacity)**: Creates a new dynamic memory pool.

- **void \*pool_dyn_alloc(PoolDyn \*pool, size_t size)**: Allocate memory from the pool.

- **void \*pool_dyn_alloc_safe(PoolDyn \*pool, size_t size)**: Allocation with automatic merging of free blocks.

- **void pool_dyn_free(PoolDyn \*pool, void \*block)**: Free previously allocated memory.

- **void pool_dyn_clear(PoolDyn \*pool)**: Clear pool.

- **void pool_dyn_destroy(PoolDyn \*pool)**: Destroys the pool and frees all associated memory.

- **size_t pool_dyn_size(PoolDyn \*pool)**: Returns the current size of the pool's occupied space.

- **size_t pool_dyn_capacity(PoolDyn \*pool)**: Returns the total size of the pool.

- **void coalesce_free_blocks(PoolDyn \*pool)**: Merges adjacent free blocks (eliminates fragmentation).

- **void restore_block(PoolDyn \*pool, void \*block)**: Restore damaged block.

### Pool logger

- **poolEnableLogToStdout(logLevel level)**: Enable logging to stdout.

- **poolDisableLogToStdout(void)**: Disable logging to stdout.

- **poolEnableLogToFile(const char \*file_name, logLevel level)**: Enable logging to file.

- **poolDisableLogToFile(void)**: Disable logging to file.
