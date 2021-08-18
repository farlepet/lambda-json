#include <stdio.h>

#include "lambda-json.h"

/* Test 1:
 *   Tests that ljson returns failure on improperly-formatted inputs. Use
 *   Valgrind to ensure data is properly free'd by ljson_destroy. */

const char *_tests[] = {
   ",",         /* Unexpected comma */
   "null,",     /* Unexpected comma, w/ initial content */
   "\"text'",   /* Mismatched quotes */
   "'text\"",
   "'text''",   /* Unmatched quote */
   "\"text\"\"",
   "1.32.1",    /* Extra decimal point */
   "[32,]",     /* Empty array index */
   "[",         /* Unclosed empty array */
   "[32,",      /* Unclosed array, with contents */
   "[32",       /* Unclosed array, with contents, no comma */
   "{'a':}",      /* No map association */
   "{'a':null,}", /* Empty map index */
   "{",           /* Unclosed empty map */
   "{'a':null,",  /* Unclosed map, with contents */
   "{'a':null"    /* Unclosed map, with contents, no comma */
};
#define N_TESTS (sizeof(_tests) / sizeof(_tests[0]))

int main() {
    int pass = 0, fail = 0;
    ljson_t *json;

    for(unsigned i = 0; i < N_TESTS; i++) {
        json = ljson_parse(_tests[i], 0);
        if(json) {
            fprintf(stderr, "\033[31mFAIL\033[0m on test %02u: %s\n", i, _tests[i]);
            ljson_destroy(json);
            fail++;
        } else {
            pass++;
            fprintf(stderr, "\033[32mPASS\033[0m on test %02u: %s\n", i, _tests[i]);
        }
    }
    
    printf("----------\n"
           "Pass: %d\n"
           "Fail: %d\n", pass, fail);

    return (fail > 0) ? -1 : 0;

    return (fail > 0) ? -1 : 0;
}
