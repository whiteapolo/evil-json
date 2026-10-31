#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include "json.h"
#include "flat-pool.h"
#include "json-string.h"

typedef struct {
    Json_String key;
    Json_Value value;
} Json_Object_Entry;

Json_Object_Entry *json_object_new(Flat_Pool *pool, size_t capacity);
static inline void json_object_push(Json_Object_Entry **object, Json_String key, Json_Value value);

#endif
