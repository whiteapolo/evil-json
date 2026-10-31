#include "print-json.h"
#include "json-array.h"
#include "json-object.h"
#include <stdio.h>

void print_json_object(Json_Value json, int level, int indentation);
void print_json_value(Json_Value value, int level, int indentation);

void print_indentation(int level, int indentation)
{
    int spaces = level * indentation;
    for (int i = 0; i < spaces; i++) {
        printf("%c", ' ');
    }
}

void print_json_array(Json_Value *array, int level, int indentation)
{
    printf("[\n");

    for (size_t i = 0; i < json_array_length(array); i++) {
        print_indentation(level + 1, indentation);
        print_json_value(array[i], level + 1, indentation);

        if (i != json_array_length(array) - 1) {
            printf(",");
        }

        printf("\n");
    }

    print_indentation(level, indentation);
    printf("]");
}

void print_json_value(Json_Value value, int level, int indentation)
{
    switch (value.type)
    {
        case JSON_NUMBER:
            printf("%g", value.number);
            break;
        case JSON_BOOLEAN:
            printf(value.boolean ? "true" : "false");
            break;
        case JSON_NULL:
            printf("null");
            break;
        case JSON_STRING:
            printf("\"%.*s\"", value.string.length, value.string.ptr);
            break;
        case JSON_ARRAY:
            print_json_array(value.array, level, indentation);
            break;
        case JSON_OBJECT:
            print_json_object(value, level, indentation);
            break;
    }
}

void print_json_object(Json_Value json, int level, int indentation)
{
    Json_Object_Entry *object = json.object;
    size_t size = json_object_size(object);

    if (size == 0) {
        printf("{}");
        return;
    }

    printf("{\n");

    for (size_t i = 0; i < size; i++) {
        print_indentation(level + 1, indentation);
        printf("\"%.*s\": ", object[i].key.length, object[i].key.ptr);
        print_json_value(object[i].value, level + 1, indentation);

        if (i < size - 1) {
            printf(",");
        }

        printf("\n");
    }

    print_indentation(level, indentation);
    printf("}");
}

void print_json(Json_Value json)
{
    print_json_value(json, 0, 4);
    printf("\n");
}
