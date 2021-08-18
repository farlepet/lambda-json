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

#define LJSON_PARSEFLAG_LENIENT (0UL << 1) /** Allow characters after parsable JSON string */

/**
 * Parse JSON-formatted string, returning an object representation.
 * 
 * @param body String to parse
 * @param flags Flags modifying the parsing, see LJSON_PARSEFLAG_*
 * 
 * @return NULL on error, else pointer to object repesenting JSON input
 */
ljson_t *ljson_parse(const char *body, uint32_t flags);

/**
 * De-allocate JSON object previously generated using ljson_parse.
 * 
 * @param json JSON object to destroy
 */
void ljson_destroy(ljson_t *json);

#ifdef __cplusplus
}
#endif

#endif
