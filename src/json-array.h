#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include "flat-pool.h"
#include "json.h"

Json_Value *json_array_new(Flat_Pool *pool, size_t capacity);
size_t json_array_length(Json_Value *array);
static inline void json_array_push(Json_Value **array, Json_Value value);

#endif
