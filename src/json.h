#ifndef JSON_H
#define JSON_H

#include "flat-pool.h"
#include "json-string.h"
#include "json-object.h"
#include <stdbool.h>

typedef enum : unsigned char {
    JSON_STRING  = 0,
    JSON_NUMBER  = 1,
    JSON_BOOLEAN = 2,
    JSON_NULL    = 3,
    JSON_OBJECT  = 4,
    JSON_ARRAY   = 5,
} Json_Value_Type;

typedef struct Json_Value Json_Value;

struct __attribute__((packed)) Json_Value {
    union {
        Json_String string;
        double number;
        void *object;
        Json_Value *array;
        bool boolean;
    };
    Json_Value_Type type;
};

Json_Value json_parse(Flat_Pool *pool, const char *document, size_t length);

#endif
