#include "flat-pool.h"
#include <stdlib.h>
#include <sys/mman.h>

Flat_Pool flat_pool_new()
{
    size_t capacity = 1024 * 1024 * 1024;
    void *ptr = mmap(NULL, capacity, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == NULL) {
        perror("flat_pool_new() Failed");
        exit(1);
    }

    Flat_Pool pool = {
        .buffer = ptr,
        .offset = 0,
        .capacity = capacity,
    };

    return pool;
}

void *flat_pool_allocate(Flat_Pool *pool, size_t size)
{
    void *ptr = pool->buffer + pool->offset;
    pool->offset += size;
    return ptr;
}

void flat_pool_free(Flat_Pool *pool)
{
    munmap(pool->buffer, pool->capacity);
}
