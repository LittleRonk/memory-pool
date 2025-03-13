# Memory Pool Implementation in C

This is a simple memory pool implementation in C. The pool allows you to allocate fixed-size blocks of memory efficiently.

## Key Features

- **Fixed-size memory blocks**: All blocks in the pool have the same size.
- **Efficient allocation**: Memory is pre-allocated, so allocation is fast.
- **Manual memory management**: You control when memory is freed or cleared.

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
./driver
```

### Example Code

Here is an example of how to use the memory pool:

```c
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"

// Example data structure to store in the pool
typedef struct {
    int id;
    char name[32];
} Data;

int main() {
    // Create a memory pool with capacity for 5 blocks of size sizeof(Data)
    size_t capacity = 5;
    size_t block_size = sizeof(Data);
    Pool *pool = pool_create(capacity, block_size);

    if (!pool) {
        printf("Error: Failed to create the memory pool.\n");
        return 1;
    }

    printf("Memory pool created successfully. Capacity: %zu, Block size: %zu\n", pool->capacity, pool->block_size);

    // Allocate a block of memory from the pool
    Data *data = (Data *)pool_alloc(pool);
    if (!data) {
        printf("Error: Failed to allocate memory from the pool.\n");
        pool_destroy(pool);
        return 1;
    }

    // Use the allocated memory
    data->id = 1;
    snprintf(data->name, sizeof(data->name), "Example Data");

    printf("Data allocated: id = %d, name = %s\n", data->id, data->name);

    // Free the memory block
    printf("Freeing the memory block...\n");
    pool_free(pool, data);

    // Clear the entire pool (optional)
    printf("Clearing the pool...\n");
    pool_clear(pool);

    // Destroy the pool and free all memory
    printf("Destroying the pool...\n");
    pool_destroy(pool);

    printf("Memory pool example completed.\n");
    return 0;
}
```
## API Overview

- **Pool \*pool_create(size_t capacity, size_t block_size)**: Creates a new memory pool.

- **void \*pool_alloc(Pool \*pool)**: Allocates a block of memory from the pool.

- **void pool_free(Pool \*pool, void \*memblock)**: Frees a previously allocated block.

- **void pool_clear(Pool \*pool)**: Frees all blocks in the pool.

- **void pool_destroy(Pool \*pool)**: Destroys the pool and frees all associated memory.
