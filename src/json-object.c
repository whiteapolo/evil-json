#include "json-object.h"

// TODO: maybe use smaller integers
typedef struct {
    Flat_Pool *pool;
    size_t length;
    size_t capacity;
} Json_Object_Header;

static inline Json_Object_Header *json_object_get_header(Json_Object_Entry *object)
{
    return ((Json_Object_Header *)object) - 1;
}

Json_Object_Entry *json_object_new(Flat_Pool *pool, size_t capacity)
{
    size_t mem_size = sizeof(Json_Object_Header) + sizeof(Json_Value) * capacity;
    Json_Object_Header *mem = flat_pool_allocate(pool, mem_size);

    mem->pool = pool;
    mem->length = 0;
    mem->capacity = capacity;

    return (Json_Object_Entry *)(mem + 1);
}

static inline void json_object_push(Json_Object_Entry **object, Json_String key, Json_Value value)
{
    Json_Object_Header *header = json_object_get_header(*object);

    if (header->length + 1 > header->capacity) {
        Json_Object_Entry *new_object = json_object_new(header->pool, header->length * 2);
        memcpy(new_object, *object, header->length * sizeof(Json_Object_Entry));
        *object = new_object;
    }

    Json_Object_Entry entry = {
        .key = key,
        .value = value,
    };

    (*object)[header->length++] = entry;
}
