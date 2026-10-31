#include "json-array.h"
#include "flat-pool.h"
#include "json.h"
#include <string.h>

#define JSON_ARRAY_GROWTH_FACTOR 2

// TODO: maybe use smaller integers
typedef struct {
    Flat_Pool *pool;
    size_t length;
    size_t capacity;
} Json_Array_Header;

static inline Json_Array_Header *json_array_get_header(Json_Value *array)
{
    return ((Json_Array_Header *)array) - 1;
}

Json_Value *json_array_new(Flat_Pool *pool, size_t capacity)
{
    size_t mem_size = sizeof(Json_Array_Header) + sizeof(Json_Value) * capacity;
    Json_Array_Header *mem = flat_pool_allocate(pool, mem_size);

    mem->pool = pool;
    mem->length = 0;
    mem->capacity = capacity;

    return (Json_Value *)(mem + 1);
}

static inline void json_array_push(Json_Value **array, Json_Value value)
{
    Json_Array_Header *header = json_array_get_header(*array);

    if (header->length + 1 > header->capacity) {
        Json_Value *new_array = json_array_new(header->pool, header->length * JSON_ARRAY_GROWTH_FACTOR);
        memcpy(new_array, *array, header->length * sizeof(Json_Value));
        *array = new_array;
    }

    (*array)[header->length++] = value;
}

size_t json_array_length(Json_Value *array)
{
    Json_Array_Header *header = json_array_get_header(array);
    return header->length;
}
