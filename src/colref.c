#include "../inc/colref.h"
#include "../inc/buffer_types.h"
#include <stdlib.h>
#include <string.h>

int colref_init(struct colref *obj, const char *res, const char *col)
{
        size_t len_res = 0;
        size_t len_col = 0;

        len_res = strlen(res);
        len_col = strlen(col);

        init_buffer(&obj->resource, len_res, buffer_type_str);
        init_buffer(&obj->colref, len_col, buffer_type_str);

        buffer_set_value(&obj->resource, res, len_res);
        buffer_set_value(&obj->colref, col, len_col);

        return 0;
}

void colref_free(struct colref *obj)
{
        destroy_buffer(&obj->resource);
        destroy_buffer(&obj->colref);
}

const char *colref_get_resource(struct colref *obj)
{
        if (obj->resource.init_flag) {
                return (char *)obj->resource.buf;
        } else {
                return NULL;
        }
}

const char *colref_get_colref(struct colref *obj)
{
        if (obj->colref.init_flag) {
                return (char *)obj->colref.buf;
        } else {
                return NULL;
        }
}
