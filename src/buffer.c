/***
 * Generic buffer class
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../inc/buffer.h"
#include "../inc/log.h"
#include "../inc/status.h"

int init_buffer(struct buffer *buf, size_t sz, enum buffer_type type)
{
        buf->init_flag = CCSVCUBE_STATUS_SUCCESS;
        buf->type = type;
        buf->sz = sz;
        buf->buf = malloc(sz);
        memset(buf->buf, 0, sz);

        return CCSVCUBE_STATUS_SUCCESS;
}

int realoc_buffer(struct buffer *buf, void *value, size_t sz)
{
        void *ptr = NULL;
        /*  Check if buffer is initialised */
        if (check_buffer_init_stat(buf)) {
                LOG_MESSAGE("Buffer not initialised");
                return CCSVCUBE_STATUS_FAILED;
        }
        ptr = realloc(get_buffer(buf), get_buffer_sz(buf) + sz + 1);

        if (ptr == NULL) {
                /*  Out of memory */
                return CCSVCUBE_STATUS_FAILED;
        }

        set_buffer_ptr(buf, ptr);
        memcpy(&(buf->buf[buf->sz]), value, sz);
        adjust_buffer_sz(buf, sz);
        memset(&buf->buf[buf->sz], 0, 1);

        return CCSVCUBE_STATUS_SUCCESS;
}

void destroy_buffer(struct buffer *buf)
{
        if (buf->init_flag != CCSVCUBE_STATUS_SUCCESS) {
                /* buffer not initialised */
                return;
        }

        if (buf->sz > 0) {
                free(buf->buf);
                buf->buf = NULL;
        }

        buf->sz = 0;
}

int buffer_set_value(struct buffer *buf, const void *value, size_t sz)
{
        if (buf->init_flag != CCSVCUBE_STATUS_SUCCESS) {
                /* buffer not initialised */
                return CCSVCUBE_STATUS_FAILED;
        }

        if (sz > buf->sz) {
                /* Buffer size exceeds initialised */
                return CCSVCUBE_STATUS_FAILED;
        }

        memcpy(buf->buf, value, sz);
        return CCSVCUBE_STATUS_SUCCESS;
}
