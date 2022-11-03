#include <stdio.h>
#include <string.h>

#include "../inc/reader.h"
#include "../inc/status.h"

#include "../inc/log.h"

#include "../inc/geojson.h"
#include "json-c/arraylist.h"
#include "json-c/json_object.h"
#include "json-c/json_types.h"
#include "uthash/utarray.h"

#define GEOJSON_PLR_BUFFER 16

static void init_json_arr(void* etl);
static void dtor_json_arr(void *etl);
static void ctor_json_arr(void *dst, const void *src);

static int geojson_write_file(struct geojson *geojson);
static int geojson_add_to_array(UT_array *arr, json_object *obj);

UT_icd geojson_json_icd = {sizeof(json_object_ut_t), init_json_arr,
                               ctor_json_arr, dtor_json_arr};

/* Intialise geojson structure */
int geojson_init(struct geojson *geojson)
{
        geojson->init_flg = CCSVCUBE_STATUS_SUCCESS;
        geojson->base_file = NULL;
        geojson->filter = NULL;
        geojson->page_sz = GEOJSON_DEFAULT_PAGE_SZ;
        geojson->total = 0;

        return CCSVCUBE_STATUS_SUCCESS;
}

/* Split into multiple files */
int geojson_split_into_multiples(struct geojson *geojson, json_object *obj)
{
        enum json_type jtype;
        json_object* jarr = NULL;
        json_object* elm = NULL;
        json_object *prop = NULL;
        json_object *plr = NULL;
        const char* plr_val = NULL;
        json_bool stat;
        size_t arr_sz = 0;
        size_t i = 0;
        char *t_plr;

        struct geojson_filter *filter = NULL;
        struct geojson_filter *entry = NULL;
        struct geojson_filter *tmp = NULL;

        json_object_ut_t *tmp_ut = NULL;

        LOG_MESSAGE("Reading JSON object");
        jtype = json_object_get_type(obj);

        if (jtype != json_type_object) {
                LOG_MESSAGE("Unable to handler non json objects");
                goto ERROR_HANDLER;
        }

        LOG_MESSAGE_ARGS("Checking for %s", GEOJSON_FEATURES_KEY);
        stat = json_object_object_get_ex(obj, GEOJSON_FEATURES_KEY, &jarr);
        if (!stat) {
                LOG_MESSAGE_ARGS("Unable to get the key %s", GEOJSON_FEATURES_KEY);
                goto ERROR_HANDLER;
        }


        LOG_MESSAGE("Checking for array count");
        arr_sz = json_object_array_length(jarr);

        LOG_MESSAGE_ARGS("Array count found %u", arr_sz);

        LOG_MESSAGE("Iterating through the array");
        for (i = 0; i < arr_sz; ++i) {
                elm = json_object_array_get_idx(jarr, i);

                if (elm == NULL)
                        continue;


                /* Get properties */
                stat = json_object_object_get_ex(elm, GEOJSON_PROPERTIES_KEY, &prop);
                if (!stat) {
                        LOG_MESSAGE_ARGS("Unable to get property %s", GEOJSON_PROPERTIES_KEY);
                        continue;
                }


                /* Get PLR */
                stat = json_object_object_get_ex(prop, GEOJSON_PLR_KEY, &plr);
                if (!stat) {
                        LOG_MESSAGE_ARGS("Unable to get property %s", GEOJSON_PLR_KEY);
                }

                entry = NULL;
                if (stat) {
                        plr_val = json_object_get_string(plr);
                        if (plr_val) {
                                LOG_MESSAGE_ARGS("PLR for arr element %u: %s", i, plr_val);
                        }
                        HASH_FIND_STR(filter, plr_val, entry);
                } else {
                        plr_val = NULL;
                }


                if (!entry) {
                        /* If the entry is NULL then add a new one */
                        entry = (struct geojson_filter *)malloc(
                                sizeof(struct geojson_filter));
                        entry->id = i;
                        entry->ptr = elm;
                        entry->arr = NULL;
                        if (!plr_val) {
                                t_plr = (char *)malloc(GEOJSON_PLR_BUFFER);
                                memset(t_plr, 0, GEOJSON_PLR_BUFFER);
                                sprintf(t_plr, "%zu", i);
                                entry->name = t_plr;
                                entry->aloc_flg = 1;
                        } else {
                                entry->aloc_flg = 0;
                                entry->name = plr_val;
                        }

                        /* Add to the collection */
                        HASH_ADD_KEYPTR(hh, filter, entry->name, strlen(entry->name), entry);
                } else {
                        /* If the array is NULL create it */
                        if (!entry->arr) {
                                entry->arr = (UT_array*) malloc(sizeof(UT_array));
                                utarray_init(entry->arr, &geojson_json_icd);

                                if (entry->ptr) {
                                  tmp_ut = (json_object_ut_t *)malloc(
                                      sizeof(json_object_ut_t));
                                  tmp_ut->obj = entry->ptr;
                                  utarray_push_back(entry->arr, tmp_ut);
                                }
                        }

                        tmp_ut = (json_object_ut_t *) malloc(sizeof(json_object_ut_t));
                        tmp_ut->obj = elm;
                        utarray_push_back(entry->arr, tmp_ut);
                }
        }

        geojson->filter = filter;
        geojson->total = HASH_COUNT(filter);

        LOG_MESSAGE("Writing files");
        geojson_write_file(geojson);

        return CCSVCUBE_STATUS_SUCCESS;

ERROR_HANDLER :
        return CCSVCUBE_STATUS_FAILED;
}

int geojson_free(struct geojson *geojson)
{
        struct geojson_filter *filter = geojson->filter;
        struct geojson_filter *entry = NULL;
        struct geojson_filter *tmp = NULL;

        LOG_MESSAGE("Freeing allocated");
        entry = NULL;

        /* free the hash table contents */
        HASH_ITER(hh, filter, entry, tmp) {
          if (entry->arr) {
                  utarray_free(entry->arr);
                  entry->arr = NULL;
          }

          if (entry->aloc_flg && entry->name)
            free(entry->name);

          entry->ptr = NULL;
          entry->name = NULL;

          HASH_DEL(filter, entry);
          free(entry);
          entry = NULL;
        }

        return CCSVCUBE_STATUS_SUCCESS;
}

/* Initialise */
static void init_json_arr(void *etl)
{
        if (!etl)
                return;
        json_object_ut_t *tmp = (json_object_ut_t *) etl;
        tmp->obj = NULL;
}

/* Destructor */
static void dtor_json_arr(void *etl)
{
        if (!etl)
                return;

        json_object_ut_t *tmp = (json_object_ut_t *)etl;
        tmp->obj = NULL;
}

/* Copy */
static void ctor_json_arr(void *_dst, const void *_src)
{
        if (!_dst || !_src)
                return;

        json_object_ut_t *dst = (json_object_ut_t *) _dst, *src = (json_object_ut_t *) _src;
        dst->obj = src->obj ? src->obj : NULL;
}

static int geojson_add_to_array(UT_array *arr, json_object *obj) {
  json_object_ut_t *p = NULL;

  LOG_MESSAGE_ARGS("Iterating through the array with count %u", utarray_len(arr));
  for (p = (json_object_ut_t *)utarray_front(arr); p != NULL;
       p = (json_object_ut_t *)utarray_next(arr, p)) {

          if (p->obj != NULL)
                  json_object_array_add(obj, p->obj);
  }

  return CCSVCUBE_STATUS_SUCCESS;
}

static int geojson_write_file(struct geojson* geojson)
{
        size_t file_len = 0;
        size_t cnt = 0;
        size_t page_cnt = 0;
        char *file_name_buff;
        struct geojson_filter *filter = geojson->filter;
        struct geojson_filter *entry = NULL;
        struct geojson_filter *tmp = NULL;
        array_list *arr = NULL;

        json_object *parent = NULL;
        json_object *obj = NULL;
        size_t num_pages = (size_t)geojson->total / geojson->page_sz;
        if (geojson->total % geojson->page_sz)
                num_pages++;

        if (geojson->base_file) {
                file_len = strlen(geojson->base_file) + 20;
        } else {
                geojson->base_file = "b";
                file_len = 20;
        }

        LOG_MESSAGE_ARGS("Number of pages: %u", num_pages);
        LOG_MESSAGE_ARGS("Page size: %u", geojson->page_sz);
        LOG_MESSAGE_ARGS("Total: %u", geojson->total);

        file_name_buff = (char*) malloc(sizeof(char) * file_len);
        memset(file_name_buff, 0, file_len);

        LOG_MESSAGE_ARGS("Hash table contains %i entries", HASH_COUNT(filter));
        parent = json_object_new_object();

        HASH_ITER(hh, filter, entry, tmp) {

          if (obj && (cnt == geojson->page_sz * page_cnt) &&
              json_object_array_length(obj) > 0) {
            LOG_MESSAGE_ARGS("Wrigin JSON array to file: %s for page %u",
                             file_name_buff, page_cnt);
            json_object_object_add(parent, "features", obj);
            json_object_to_file(file_name_buff, parent);
          }

          if (obj == NULL || !(cnt % geojson->page_sz)) {
                  LOG_MESSAGE("Creating new JSON array");
                  obj = json_object_new_array_ext(geojson->page_sz);

                  sprintf(file_name_buff, "%s-%zu.geojson", geojson->base_file,
                          ++page_cnt);

                  LOG_MESSAGE_ARGS("File name: %s", file_name_buff);
          }

          if (entry->arr) {
                  LOG_MESSAGE("Related pipes encounted, handling array");
                  geojson_add_to_array(entry->arr, obj);
          } else {
                  json_object_array_add(obj, entry->ptr);
          }

          cnt++;
        }

        if (obj && json_object_array_length(obj) > 0) {
                LOG_MESSAGE_ARGS("Wrigin JSON array to file: %s for page %u and object count: %u",
                                 file_name_buff,
                                 page_cnt,
                                 json_object_array_length(obj));
                json_object_object_add(parent, "features", obj);
                json_object_to_file(file_name_buff, parent);
        }

         free(file_name_buff);

         return CCSVCUBE_STATUS_SUCCESS;
}
