#include <stdio.h>
#include <stdlib.h>

#include "json-object.h"
#include "json.h"
#include "flat-pool.h"
#include "json-string.h"
#include "json-array.h"
#include "time.h"

#include "json.c"
#include "flat-pool.c"
#include "json-array.c"
#include "json-object.c"
#include "time.c"

size_t get_file_size(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    return size;
}

bool read_file(Flat_Pool *pool, const char *pathname, Json_String *out)
{
    FILE *fp = fopen(pathname, "r");

    if (fp == NULL) {
        return false;
    }

    size_t length = get_file_size(fp);
    char *ptr = flat_pool_allocate(pool, length);
    fread(ptr, length, 1, fp);

    out->ptr = ptr;
    out->length = length;

    return true;
}

int main()
{
    Flat_Pool pool = flat_pool_new();
    Json_String document;


    if (!read_file(&pool, "data/20MB.json", &document)) {
        perror("Opps");
        exit(1);
    }

    Time start = get_time();
    Json_Value value = json_parse(&pool, document.ptr, document.length);
    time_print_elapsed_mseconds(start);

    double mb_per_second = document.length / time_elapsed_seconds(start) / 1000000.0;
    printf("(%g MB/s)\n", mb_per_second);

    // printf("%d\n", value.type);

    // printf("%.*s\n", document.length, document.ptr);

    return 0;
}
