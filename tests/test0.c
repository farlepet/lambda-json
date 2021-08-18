#include <stdio.h>

#include "lambda-json.h"

/* Test 0:
 *   Tests that ljson returns success on properly-formatted inputs. Does NOT
 *   test for the validity of the resultant struct. Use Valgrind to ensure
 *   data is properly free'd by ljson_destroy. */

const char *_tests[] = {
    "0",                 /* Integer */
    "-24",               /* Negative integer */
    "1.24",              /* Floating point */
    "-3.14",             /* Negative floating point */
    "\"str\"",           /* String with " */
    "\'str\'",           /* String with ' */
    "null",              /* Null */
    "[0,1]",             /* Array */
    "{'0':2.4,'1':1}",   /* Map with ' */
    "{\"0\":0,\"1\":1}", /* Map with "" */

    "[[[[0,1],[2,3]],[]]]", /* Deep array, with empty entry */
    "{'0':{'a':{'A':{'_':null}},'b':{},'c':24}}" /* Deep map, with empty entry */
};
#define N_TESTS (sizeof(_tests) / sizeof(_tests[0]))

int main() {
    int pass = 0, fail = 0;
    ljson_t *json;

    for(unsigned i = 0; i < N_TESTS; i++) {
        json = ljson_parse(_tests[i]);
        if(!json) {
            fprintf(stderr, "json_parse[%u]: %s FAIL\n", i, _tests[i]);
            fail++;
        } else {
            pass--;
            fprintf(stderr, "json_parse[%u]: %s PASS\n", i, _tests[i]);
            ljson_destroy(json);
        }
    }

    return (fail > 0) ? -1 : 0;
}
