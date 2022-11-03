/* Implementation of list class
 */
#include "../inc/list.h"
#include "../inc/buffer.h"
#include "../inc/status.h"

#include "uthash/utarray.h"


static void list_ut_copy(void *_dst, const void* _src)
{
        struct buffer* dst = (struct buffer *) _dst;
        struct buffer* src = (struct buffer *) _src;

        buffer_set_value(dst, src->buf, src->sz);
}

static void list_ut_destroy(void *_src)
{
        struct buffer* src = (struct buffer *) _src;
        destroy_buffer(src);
}

static UT_icd list_buf_icd = {sizeof(struct buffer), NULL, list_ut_copy, list_ut_destroy };

int list_init(struct list* list)
{
        list->init_flg = CCSVCUBE_STATUS_SUCCESS;
        list->count = 0;
        utarray_new(list->arr, &list_buf_icd);

        return CCSVCUBE_STATUS_SUCCESS;
}


void list_delete(struct list* list)
{
        if(list->init_flg != CCSVCUBE_STATUS_SUCCESS) {
                return;
        }
        list->init_flg = CCSVCUBE_STATUS_FAILED;
        list->count = 0;
        utarray_free(list->arr);
}


int list_add_item(struct list* list, void* obj)
{
        CUTILS_CHECK_INIT(list);
        utarray_push_back(list->arr, obj);
        list->count++;

        return CCSVCUBE_STATUS_SUCCESS;
}


int list_remove_item(struct list* list, void *obj)
{
        CUTILS_CHECK_INIT(list);
        list->count--;
        return CCSVCUBE_STATUS_FAILED;
}

void* list_get_item(struct list* list, size_t ix)
{
        struct buffer* buf = NULL;
        void *tmp = NULL;
        if (list->init_flg != CCSVCUBE_STATUS_SUCCESS) {
                return NULL;
        }

        tmp = utarray_eltptr(list->arr, ix);
        if (!tmp)
                return NULL;

        buf = (struct buffer *) tmp;
        return buf->buf;
}


size_t list_get_item_count(struct list* list)
{
        return list->count;
}
