#include <stdio.h>

#include "lambda-json.h"

/* Test 3:
 *   Tests ljson decode flags - validity only. */

static const struct {
    const char *json;
    uint32_t    flags;
    int         result;
} _tests[] = {
    { "{'a':1,'b':2}",
      0x00000000, 1 },
    { "{'a':1,'b':2}",
      0x00000001, 1 },
    { "{'a':1,'b':2}a",
      0x00000000, 0 },
    { "{'a':1,'b':2}a",
      0x00000001, 1 },
    { "a{'a':1,'b':2}",
      0x00000000, 0 },
    { "a{'a':1,'b':2}",
      0x00000001, 0 },
};
#define N_TESTS (sizeof(_tests) / sizeof(_tests[0]))

int main() {
    int pass = 0, fail = 0;
    ljson_t *json;

    printf("Test 2: Test functionality of LJSON_PARSEFLAG_LENIENT\n"
           "----------\n");

    for(unsigned i = 0; i < N_TESTS; i++) {
        json = ljson_parse(_tests[i].json, _tests[i].flags);
        if((_tests[i].result  && !json) ||
           (!_tests[i].result &&  json)) {
            fprintf(stderr, "\033[31mFAIL\033[0m on test %02u: %s\n", i, _tests[i].json);
            fail++;
        } else {
            pass++;
            fprintf(stderr, "\033[32mPASS\033[0m on test %02u: %s\n", i, _tests[i].json);
        }

        if(json) {
            ljson_destroy(json);
        }
    }

    printf("----------\n"
           "Pass: %d\n"
           "Fail: %d\n", pass, fail);

    return (fail > 0) ? -1 : 0;
}
