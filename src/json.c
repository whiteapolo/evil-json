#include "json.h"
#include "flat-pool.h"
#include "json-array.h"
#include "json-object.h"
#include "json-string.h"
#include "print-json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MIN_CAPACITY 16

typedef struct {
    Json_String document;
    size_t curr;
    Flat_Pool *pool;
} Json_Parse_State;

Json_Value json_parse_value(Json_Parse_State *state);

static inline bool json_is_at_end(Json_Parse_State *state)
{
    return state->curr == state->document.length;
}

static inline char json_peek(Json_Parse_State *state)
{
    return state->document.ptr[state->curr];
}

static inline char json_check(Json_Parse_State *state, char expected)
{
    return json_peek(state) == expected;
}

static inline bool json_check_string(Json_Parse_State *state, Json_String expected)
{
    if (state->document.length - state->curr < expected.length) {
        return false;
    }

    return strncmp(state->document.ptr + state->curr, expected.ptr, expected.length) == 0;
}

static inline char json_advance(Json_Parse_State *state)
{
    return state->document.ptr[state->curr++];
}

static inline void json_advance_n(Json_Parse_State *state, size_t n)
{
    state->curr += n;
}

static inline bool is_space(char c)
{
    return c == ' '
        || c == '\n'
        || c == '\t'
        || c == '\r';
}

static inline void json_skip_spaces(Json_Parse_State *state)
{
    while (!json_is_at_end(state) && is_space(json_peek(state))) {
        json_advance(state);
    }
}

static inline bool json_match(Json_Parse_State *state, char expected)
{
    if (json_check(state, expected)) {
        json_advance(state);
        return true;
    }

    return false;
}

static inline char to_lower(char c)
{
    return 'A' <= c && c <= 'Z' ? c + 'a' - 'A' : c;
}

static inline bool json_match_case_insensitive(Json_Parse_State *state, char expected)
{
    return json_match(state, to_lower(expected));
}

static inline void json_die(Json_Parse_State *state)
{
    char buf[256];
    strncpy(buf, state->document.ptr + state->curr, 256);
    fprintf(stderr, "Failed at curr=%zu\nline: @%s@\n", state->curr, buf);
    exit(1);
}

static inline void json_expect(Json_Parse_State *state, char expected)
{
    if (!json_match(state, expected)) {
        json_die(state);
    }
}

static inline bool json_match_string(Json_Parse_State *state, Json_String expected)
{
    if (json_check_string(state, expected)) {
        json_advance_n(state, expected.length);
        return true;
    }

    return false;
}

static inline bool is_number_character(char c)
{
    switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case 'e': case '-': case '+':
            return true;

        default:
            return false;
    }
}

static inline Json_Value create_json_null()
{
    Json_Value value = {
        .type = JSON_NULL,
    };

    return value;
}

static inline Json_Value create_json_string(Json_String string)
{
    Json_Value value = {
        .type = JSON_STRING,
        .string = string,
    };

    return value;
}

static inline Json_Value create_json_number(double number)
{
    Json_Value value = {
        .type = JSON_NUMBER,
        .number = number,
    };

    return value;
}

static inline Json_Value create_json_boolean(bool boolean)
{
    Json_Value value = {
        .type = JSON_BOOLEAN,
        .boolean = boolean,
    };

    return value;
}

static inline Json_Value create_json_array(Json_Value *array)
{
    Json_Value value = {
        .type = JSON_ARRAY,
        .array = array,
    };

    return value;
}

static inline Json_Value create_json_object(Json_Object_Entry *object)
{
    Json_Value value = {
        .type = JSON_OBJECT,
        .object = object,
    };

    return value;
}

static inline Json_Value json_parse_string(Json_Parse_State *state)
{
    json_expect(state, '"');

    const char *start = state->document.ptr + state->curr;
    const char *end = memchr(state->document.ptr + state->curr, '"', state->document.length - state->curr);

    if (end == NULL) {
        json_die(state);
    }

    state->curr += end - start + 1;

    Json_String string = {
        .ptr = start,
        .length = end - start,
    };

    return create_json_string(string);
}

static inline bool is_digit(char c)
{
    return '0' <= c && c <= '9';
}

static inline Json_Value json_parse_number(Json_Parse_State *state)
{
    int num = 0;
    int sign = 1;

    if (json_match(state, '-')) {
        sign = -1;
    } else {
        json_match(state, '+');
    }

    while (is_digit(json_peek(state))) {
        num = num * 10 + json_advance(state) - '0';
    }

    if (!json_match(state, '.')) {
        return create_json_number(sign * num);
    }

    double fraction = 0;
    int i = 1;

    while (is_digit(json_peek(state))) {
        fraction += (json_advance(state) - '0') / i * 10;
    }

    if (json_match_case_insensitive(state, 'e')) {
        int exponent_sign = 1;
        int exponent = 0;

        if (json_match(state, '-')) {
            sign = -1;
        } else {
            json_match(state, '+');
        }

        while (is_digit(json_peek(state))) {
            exponent = exponent * 10 + json_advance(state) - '0';
        }

        double result = (sign * ((double)num + fraction)) * pow(10, exponent * exponent_sign);
        return create_json_number(result);
    }

    return create_json_number(sign * ((double)num + fraction));
}

static inline Json_Value json_parse_true(Json_Parse_State *state)
{
    if (json_match_string(state, JSON_STRING_LIT("true"))) {
        return create_json_boolean(true);
    }

    json_die(state);
}

static inline Json_Value json_parse_false(Json_Parse_State *state)
{
    if (json_match_string(state, JSON_STRING_LIT("false"))) {
        return create_json_boolean(false);
    }

    json_die(state);
}

static inline Json_Value json_parse_null(Json_Parse_State *state)
{
    if (json_match_string(state, JSON_STRING_LIT("null"))) {
        return create_json_null();
    }

    json_die(state);
}

static inline Json_Value json_parse_array(Json_Parse_State *state)
{
    json_expect(state, '[');
    json_skip_spaces(state);

    Json_Value *array = json_array_new(state->pool, MIN_CAPACITY);

    if (json_match(state, ']')) {
        return create_json_array(array);
    }

    do {
        json_skip_spaces(state);
        json_array_push(&array, json_parse_value(state));
        json_skip_spaces(state);
    } while (json_match(state, ','));

    if (!json_match(state, ']')) {
        json_die(state);
    }

    return create_json_array(array);
}

static inline Json_Value json_parse_object(Json_Parse_State *state)
{
    json_expect(state, '{');
    json_skip_spaces(state);

    Json_Object_Entry *object = json_object_new(state->pool, MIN_CAPACITY);

    if (json_match(state, '}')) {
        return create_json_object(object);
    }

    do {
        json_skip_spaces(state);
        Json_Value key = json_parse_string(state);
        json_skip_spaces(state);
        json_expect(state, ':');
        json_skip_spaces(state);
        Json_Value value = json_parse_value(state);
        json_skip_spaces(state);

        json_object_push(&object, key.string, value);
    } while (json_match(state, ','));

    json_expect(state, '}');

    return create_json_object(object);
}

Json_Value json_parse_value(Json_Parse_State *state)
{
    json_skip_spaces(state);

    switch (json_peek(state)) {
        case '"': return json_parse_string(state);
        case '[': return json_parse_array(state);
        case '{': return json_parse_object(state);

        case 't': return json_parse_true(state);
        case 'f': return json_parse_false(state);
        case 'n': return json_parse_null(state);

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case 'e': case '-': case '+':
            return json_parse_number(state);

        default: json_die(state);
    }
}

Json_Value json_parse(Flat_Pool *pool, const char *document, size_t length)
{
    Json_Parse_State state = {
        .pool = pool,
        .curr = 0,
        .document = {
            .ptr = document,
            .length = length,
        },
    };

    return json_parse_value(&state);
}

