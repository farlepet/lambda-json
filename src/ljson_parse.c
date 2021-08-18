#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lambda-json.h"

#if defined(LJSON_DEBUG)
#  include <stdio.h>
#  define DEBUG_PRINT(STR, ...) fprintf(stderr, "ljson debug: "STR"\n", __VA_ARGS__)
#else
#  define DEBUG_PRINT(STR, ...)
#endif

static int         _ljson_item_parse(const char *, const char **, ljson_item_t *);
static void        _ljson_item_delete(ljson_item_t *);
static const char *_skipwht(const char *);

ljson_t *ljson_parse(const char *body, uint32_t flags) {
    ljson_t *json = (ljson_t *)malloc(sizeof(ljson_t));

    const char *end = body;

    if(_ljson_item_parse(body, &end, &json->root)) {
        DEBUG_PRINT("Parsing failed around position %lu", (end - body));
        free(json);
        return NULL;
    }

    if(!(flags & LJSON_PARSEFLAG_LENIENT)) {
        /* Check that we are at the end of the input */
        end = _skipwht(end);
        if(*end != '\0') {
            ljson_destroy(json);
            return NULL;
        }
    }

    return json;
}

void ljson_destroy(ljson_t *json) {
    _ljson_item_delete(&json->root);
    free(json);
}

/**
 * Deallocates memory used within the item, but NOT the item struct itself.
 */
static void _ljson_item_delete(ljson_item_t *item) {
    switch(item->type) {
        case LJSON_ITEMTYPE_STRING:
            free(item->str);
            break;

        case LJSON_ITEMTYPE_ARRAY:
            for(uint16_t i = 0; i < item->array->count; i++) {
                _ljson_item_delete(&item->array->items[i]);
            }
            free(item->array);
            break;

        case LJSON_ITEMTYPE_MAP:
            for(uint16_t i = 0; i < item->map->count; i++) {
                _ljson_item_delete(&item->map->items[i].item);
                free(item->map->items[i].name);
            }
            free(item->map);
            break;

        case LJSON_ITEMTYPE_NONE:
        case LJSON_ITEMTYPE_NULL:
        case LJSON_ITEMTYPE_INTEGER:
        case LJSON_ITEMTYPE_FLOAT:
            /* Nothing is allocated for these types */
            break;
    }
}

/**
 * Checks if character is whitespace
 */
static int _iswht(char ch) {
    return ((ch == ' ')  ||
            (ch == '\t') ||
            (ch == '\r') ||
            (ch == '\n'));
}

/**
 * Skips whitespace characters in string, and returns pointer to first
 * non-whitespace character.
 */
static const char *_skipwht(const char *text) {
    while(_iswht(*text)) text++;
    return text;
}

static int _ljson_item_parse_number(const char *body, const char **end, ljson_item_t *item) {
    unsigned i = 0;
    if(body[i] == '-' || body[i] == '+') i++;
    while(isdigit(body[i])) i++;
    /* To comply with strto* signature */
    char *_end;

    if(body[i] == '.') {
        item->type = LJSON_ITEMTYPE_FLOAT;
        /* @todo Actually fix pointer type, rather than simply casting */
        item->flt  = (LJSON_FLOATTYPE)strtod(body, &_end);
        DEBUG_PRINT("float: %lf", item->flt);
    } else {
        item->type    = LJSON_ITEMTYPE_INTEGER;
        item->integer = (LJSON_INTTYPE)strtol(body, &_end, 10);
        DEBUG_PRINT("integer: %d", item->integer);
    }

    *end = _end;

    return 0;
}

static int _count_items(const char *body) {
    /* @note This could be simplified by simply making arrays and maps dynamic
     * lists. Major concern is performance impact, both in speed an memory
     * utilization - this needs to be examined first */
    const char *next = _skipwht(body+1);
    if((*body == '{' && *next == '}') ||
       (*body == '[' && *next == ']')) {
        /* No items */
        return 0;
    }
    
    uint16_t count = 1;
    uint16_t depth = 1;
    body++;

    while(depth && *body) {
        /* @todo This is a little lenient, you could easily end an array with a
         * curly brace, and this wouldn't care. */
        if((*body == '[') ||
           (*body == '{')) {
            depth++;
        } else if((*body == ']') ||
                  (*body == '}')) {
            depth--;
        } else if((depth == 1) &&
                  (*body == ',')) {
            /* Count only commas within the depth of the array. */
            count++;
        }

        body++;
    }

    if(depth) {
        /* Could not find end of array */
        return -1;
    }

    return count;
}

static int _ljson_item_parse_array(const char *body, const char **end, ljson_item_t *item) {
    item->type = LJSON_ITEMTYPE_ARRAY;

    int fail = 0;
    int count = _count_items(body);
    if(count < 0) {
        return -1;
    }
    DEBUG_PRINT("array item count: %d", count);
    body++;

    item->array = (ljson_array_t *)malloc(sizeof(ljson_array_t) + ((size_t)count * sizeof(ljson_item_t)));
    if(!item->array) {
        return -1;
    }
    item->array->count = 0;

    int i = 0;
    for(; i < count; i++) {
        if(_ljson_item_parse(body, end, &item->array->items[i])) {
            fail = 1;
            break;
        } else {
            /* We increment this one at a time, so delete can still work */
            item->array->count = (uint16_t)(i + 1);
            body = *end;
            body = _skipwht(body);
            if(*body != ',') {
                /* End of array, or bad formatting */
                break;
            }
            body++;
        }
    }
    
    body = _skipwht(body);

    if((count == 0 && i != 0)         ||
       (count      && i != (count-1)) ||
       (*body != ']')                 ||
       fail) {
        DEBUG_PRINT("array fail: (%d, %d), %c, %d", i, count, *body, fail);
        _ljson_item_delete(item);
        return -1;
    }

    *end = body + 1;

    return 0;
}

static int _ljson_parse_mapitem(const char *body, const char **end, ljson_mapitem_t *mapitem) {
    body = _skipwht(body);
    char strch = *body;
    if((strch != '"') &&
       (strch != '\'')) {
        return -1;
    }
    body++;
    size_t len = 0;
    while(body[len] != strch) {
        if(body[len] == '\0') {
            return -1;
        }
        len++;
    }
    mapitem->name = strndup(body, len);
    if(!mapitem->name) {
        return -1;
    }

    body = _skipwht(&body[len + 1]);
    if(*body != ':') {
        free(mapitem->name);
        return -1;
    }
    body++;

    if(_ljson_item_parse(body, end, &mapitem->item)) {
        free(mapitem->name);
        return -1;
    }

    return 0;
}

static int _ljson_item_parse_map(const char *body, const char **end, ljson_item_t *item) {
    item->type = LJSON_ITEMTYPE_MAP;

    int fail = 0;
    int count = _count_items(body);
    if(count < 0) {
        return -1;
    }
    DEBUG_PRINT("map item count: %d", count);
    body++;

    item->map = (ljson_map_t *)malloc(sizeof(ljson_map_t) + ((size_t)count * sizeof(ljson_mapitem_t)));
    if(!item->map) {
        return -1;
    }
    item->map->count = 0;
    
    int i = 0;
    for(; i < count; i++) {
        if(_ljson_parse_mapitem(body, end, &item->map->items[i])) {
            fail = 1;
            break;
        } else {
            /* We increment this one at a time, so delete can still work */
            item->map->count = (uint16_t)(i + 1);
            body = *end;
            body = _skipwht(body);
            if(*body != ',') {
                /* End of map, or bad formatting */
                break;
            }
            body++;
        }
    }
    
    body = _skipwht(body);

    if((count == 0 && i != 0)         ||
       (count      && i != (count-1)) ||
       (*body != '}')                 ||
       fail) {
        DEBUG_PRINT("map fail: (%d, %d), %c, %d", i, count, *body, fail);
        _ljson_item_delete(item);
        return -1;
    }

    *end = body + 1;

    return 0;
}

static int _ljson_item_parse_string(const char *body, const char **end, ljson_item_t *item) {
    item->type  = LJSON_ITEMTYPE_STRING;
    char endchr = *body;
    body++;

    size_t sz = 0;
    while(body[sz] != endchr) {
        if(body[sz] == '\0') {
            /* Did not find the end of string */
            return -1;
        }
        sz++;
    }

    item->str = strndup(body, sz);
    if(!item->str) {
        return -1;
    }
    
    *end = &body[sz + 1];
    return 0;
}


static int _ljson_item_parse(const char *body, const char **end, ljson_item_t *item) {
    body = _skipwht(body);

    DEBUG_PRINT("_ljson_item_parse: %p, %p, %p", body, end, item);

    int ret = -1;

    if(isdigit(*body) || *body == '-' || *body == '+') {
        ret = _ljson_item_parse_number(body, end, item);
    } else if(*body == '[') {
        ret = _ljson_item_parse_array(body, end, item);
    } else if(*body == '{') {
        ret = _ljson_item_parse_map(body, end, item);
    } else if((*body == '"') ||
              (*body == '\'')) {
        ret = _ljson_item_parse_string(body, end, item);
    } else if(!strncasecmp(body, "null", 4)) {
        /* Perhaps a little too lenient on case? */
        item->type = LJSON_ITEMTYPE_NULL;
        *end       = body + 4;
        ret = 0;
    }
    
    if(!ret) {
        DEBUG_PRINT("parsed: %.*s", (int)(*end - body), body);
    }

    return ret;
}
