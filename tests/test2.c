#include <string.h>
#include <stdio.h>

#include "lambda-json.h"

/* Test 2:
 *   Tests the validity of parsed JSON objects. */

static ljson_array_t _arr0 = {
    .count = 2,
    .items = {
      {  .type    = LJSON_ITEMTYPE_INTEGER,
         .integer = 32 },
      {  .type    = LJSON_ITEMTYPE_INTEGER,
         .integer = -24 }
    }
};

static ljson_map_t _map0 = {
    .count = 2,
    .items = {
      {  .name = "a",
         .item =
           { .type    = LJSON_ITEMTYPE_INTEGER,
             .integer = 32 } },
      {  .name = "b",
         .item =
           { .type    = LJSON_ITEMTYPE_FLOAT,
             .flt     = -12.34 } } }
};

static const struct {
    const char        *json;
    const ljson_item_t object;
} _tests[] = {
    { "1234",
      { .type    = LJSON_ITEMTYPE_INTEGER,
        .integer = 1234 } },
    { "12.34",
      { .type    = LJSON_ITEMTYPE_FLOAT,
        .flt     = 12.34 } },
    { "\"12.34\"",
      { .type    = LJSON_ITEMTYPE_STRING,
        .str     = "12.34" } },
    { "'test'",
      { .type    = LJSON_ITEMTYPE_STRING,
        .str     = "test" } },
    { "\"\\\"test\"",
      { .type    = LJSON_ITEMTYPE_STRING,
        .str     = "\"test" } },
    { "'\\'test'",
      { .type    = LJSON_ITEMTYPE_STRING,
        .str     = "'test" } },
    { "'\\\\test'",
      { .type    = LJSON_ITEMTYPE_STRING,
        .str     = "\\test" } },
    { "null",
      { .type    = LJSON_ITEMTYPE_NULL } },
    { "[ 32, -24 ]",
      { .type    = LJSON_ITEMTYPE_ARRAY, 
        .array   = &_arr0 } },
    { "{ 'a': 32, 'b': -12.34 }",
      { .type    = LJSON_ITEMTYPE_MAP, 
        .map     = &_map0 } }
};
#define N_TESTS (sizeof(_tests) / sizeof(_tests[0]))

#define ABS(X) (((X) >= 0) ? (X) : -(X))

static int _check(const ljson_item_t *, const ljson_item_t *);

int main() {
    int pass = 0, fail = 0;
    ljson_t *json;

    for(unsigned i = 0; i < N_TESTS; i++) {
        json = ljson_parse(_tests[i].json, 0);
        if(!json ||
           !_check(&json->root, &_tests[i].object)) {
            fprintf(stderr, "\033[31mFAIL\033[0m on test %02u: %s\n", i, _tests[i].json);
            fail++;
        } else {
            pass++;
            fprintf(stderr, "\033[32mPASS\033[0m on test %02u: %s\n", i, _tests[i].json);
            ljson_destroy(json);
        }
    }

    printf("----------\n"
           "Pass: %d\n"
           "Fail: %d\n", pass, fail);

    return (fail > 0) ? -1 : 0;
}

static int _check(const ljson_item_t *result, const ljson_item_t *expected) {
    if(result->type != expected->type) {
        return 0;
    }
    
    switch(result->type) {
        case LJSON_ITEMTYPE_STRING:
            if(strcmp(result->str, expected->str)) {
                fprintf(stderr, "[%s](%lu) != [%s](%lu)\n",
                    result->str, strlen(result->str),
                    expected->str, strlen(expected->str));
            }
            return !strcmp(result->str, expected->str);
        
        case LJSON_ITEMTYPE_INTEGER:
            return result->integer == expected->integer;
        
        case LJSON_ITEMTYPE_FLOAT:
            return ABS(result->flt - expected->flt) < 0.0001;
        
        case LJSON_ITEMTYPE_ARRAY:
            if(result->array->count != expected->array->count) {
                return 0;
            }
            for(uint16_t i = 0; i < result->array->count; i++) {
                if(!_check(&result->array->items[i], &expected->array->items[i])) {
                    return 0;
                }
            }
            return 1;

        case LJSON_ITEMTYPE_MAP:
            if(result->map->count != expected->map->count) {
                return 0;
            }
            for(uint16_t i = 0; i < result->map->count; i++) {
                if(strcmp(result->map->items[i].name, expected->map->items[i].name) ||
                   !_check(&result->map->items[i].item, &expected->map->items[i].item)) {
                    return 0;
                }
            }
            return 1;
        
        case LJSON_ITEMTYPE_NULL:
        case LJSON_ITEMTYPE_NONE:
            return 1;
    }

    return 0;
}
