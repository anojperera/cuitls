#ifndef __GEOJSON_H__
#define __GEOJSON_H__

#include <stdio.h>
#include <stdlib.h>

#include "json-c/arraylist.h"
#include "json-c/json.h"
#include "json-c/json_object.h"
#include "json-c/json_object_iterator.h"
#include "json-c/json_types.h"
#include "uthash/utarray.h"
#include "uthash/uthash.h"

/* Wrapper for JSON object */
typedef struct {
        json_object *obj;
} json_object_ut_t;

struct geojson_filter {
        const char *name;
        int id;
        json_object *ptr;
        UT_array *arr;
        UT_hash_handle hh;
};

struct geojson {
        int init_flg;
        char *base_file;
        size_t total;
        size_t page_sz;
        struct geojson_filter *filter;
};

#define GEOJSON_FEATURES_KEY "features"
#define GEOJSON_PROPERTIES_KEY "properties"
#define GEOJSON_PLR_KEY "plr"

#define GEOJSON_DEFAULT_PAGE_SZ 2000

/* initialise geojson */
int geojson_init(struct geojson *geojson);

int geojson_split_into_multiples(struct geojson *geojson, json_object* obj);

/* Free Memory */
int geojson_free(struct geojson *geojson);

#endif /* __GEOJSON_H__ */
