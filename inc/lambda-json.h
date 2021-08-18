#ifndef LIB_LAMBDA_JSON_H
#define LIB_LAMBDA_JSON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LJSON_INTTYPE
#  define LJSON_INTTYPE   int
#endif
#ifndef LJSON_FLOATTYPE
#  define LJSON_FLOATTYPE double
#endif

typedef struct ljson_mapitem_struct ljson_mapitem_t;
typedef struct ljson_map_struct     ljson_map_t;
typedef struct ljson_array_struct   ljson_array_t;
typedef struct ljson_item_struct    ljson_item_t;
typedef struct ljson_struct         ljson_t;

typedef enum {
    LJSON_ITEMTYPE_NONE = 0,
    LJSON_ITEMTYPE_NULL,
    LJSON_ITEMTYPE_STRING,
    LJSON_ITEMTYPE_INTEGER,
    LJSON_ITEMTYPE_FLOAT,
    LJSON_ITEMTYPE_ARRAY,
    LJSON_ITEMTYPE_MAP
} ljson_itemtype_e;

struct ljson_item_struct {
    ljson_itemtype_e type;
    union {
        char           *str;
        LJSON_INTTYPE   integer;
        LJSON_FLOATTYPE flt;
        ljson_map_t    *map;
        ljson_array_t  *array;
    };
};

struct ljson_mapitem_struct {
    char        *name;
    ljson_item_t item;
};

struct ljson_map_struct {
    uint16_t        count;
    ljson_mapitem_t items[];
};

struct ljson_array_struct {
    uint16_t     count;
    ljson_item_t items[];
};

/**
 * Represents an entire JSON file
 */
struct ljson_struct {
    ljson_item_t root;
};


ljson_t *ljson_parse(const char *);

void ljson_destroy(ljson_t *);

#ifdef __cplusplus
}
#endif

#endif
