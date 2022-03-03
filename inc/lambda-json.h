#ifndef LIB_LAMBDA_JSON_H
#define LIB_LAMBDA_JSON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* @note If these are changed, they MUST also be changed when compiling the
 * library. */
#ifndef LJSON_INTTYPE
/** Type to use for storing JSON integers */
#  define LJSON_INTTYPE   int
#endif
#ifndef LJSON_FLOATTYPE
/** Type to use for storing JSON floating-points */
#  define LJSON_FLOATTYPE double
#endif

typedef struct ljson_mapitem_struct ljson_mapitem_t;
typedef struct ljson_map_struct     ljson_map_t;
typedef struct ljson_array_struct   ljson_array_t;
typedef struct ljson_item_struct    ljson_item_t;
typedef struct ljson_struct         ljson_t;

/**
 * JSON object types */
typedef enum {
    LJSON_ITEMTYPE_NONE = 0, /** Invalid */
    LJSON_ITEMTYPE_NULL,     /** null */
    LJSON_ITEMTYPE_STRING,   /** "..." or '...' */
    LJSON_ITEMTYPE_INTEGER,  /** Whole number */
    LJSON_ITEMTYPE_FLOAT,    /** Number with decimal portion */
    LJSON_ITEMTYPE_ARRAY,    /** [ ... ] */
    LJSON_ITEMTYPE_MAP       /** { "...": ... } */
} ljson_itemtype_e;

/**
 * Represents a single JSON object */
struct ljson_item_struct {
    ljson_itemtype_e type;       /** Type of this object */
    union {
        char           *str;     /** String data */
        LJSON_INTTYPE   integer; /** Integer data */
        LJSON_FLOATTYPE flt;     /** Floating-point data */
        ljson_map_t    *map;     /** Map { "...": ... } */
        ljson_array_t  *array;   /** Array [ ... ] */
    };
};

/**
 * Represents a single key-value pair in a JSON map */
struct ljson_mapitem_struct {
    char        *name; /** Key */
    ljson_item_t item; /** Value */
};

/**
 * Represents a JSON map */
struct ljson_map_struct {
    uint16_t        count;   /** Number of mappings in map */
    ljson_mapitem_t items[]; /** Mappings */
};

/**
 * Represents a JSON array */
struct ljson_array_struct {
    uint16_t     count;   /** Number of items in array */
    ljson_item_t items[]; /** Array items */
};

/**
 * Represents an entire JSON file */
struct ljson_struct {
    ljson_item_t root;
};

#define LJSON_PARSEFLAG_LENIENT (1UL << 0) /** Allow characters after parsable JSON string */

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

/**
 * Search for item corresponding to the given key within a map.
 * 
 * @param map Map to search through
 * @param key Key to search for
 * 
 * @return NULL if not found, else pointer to corresponding item
 */
ljson_item_t *ljson_map_search(ljson_map_t *map, const char *key);

/**
 * Search for item corresponding to the given key within a map, and check if it's
 * of the expected type. @see ljson_map_search
 */
static inline ljson_item_t *ljson_map_search_type(ljson_map_t *map, const char *key, ljson_itemtype_e type) {
    ljson_item_t *_item = ljson_map_search(map, key);
    if(_item && (_item->type == type)) {
        return _item;
    } else {
        return NULL;
    }
}


#ifdef __cplusplus
}
#endif

#endif
