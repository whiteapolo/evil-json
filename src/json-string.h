#ifndef JSON_STRING_H
#define JSON_STRING_H

#define JSON_STRING_LIT(lit) ((Json_String){ .ptr = lit, .length = sizeof(lit) - 1 })

typedef struct {
    const char *ptr;
    size_t length;
} Json_String;

#endif
