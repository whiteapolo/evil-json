#ifndef FLAT_POOL_H
#define FLAT_POOL_H

typedef struct {
    char *buffer;
    size_t offset;
    size_t capacity;
} Flat_Pool;

Flat_Pool flat_pool_new();
void *flat_pool_allocate(Flat_Pool *pool, size_t size);
void flat_pool_free(Flat_Pool *pool);

#endif
