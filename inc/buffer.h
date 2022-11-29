#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "buffer_types.h"
#include "status.h"
#include <stdlib.h>

struct buffer {
        unsigned char init_flag;
        enum buffer_type type;
        size_t sz;
        void *buf;
};

#define get_buffer(obj) (obj)->buf
#define get_buffer_sz(obj) (obj)->sz
#define set_buffer_ptr(obj, ptr) (obj)->buf = ptr
#define check_buffer_init_stat(obj)                                            \
        ((obj)->init_flag == CCSVCUBE_STATUS_SUCCESS ? 1 : 0)
#define adjust_buffer_sz(obj, size) (obj)->sz += size

/***
 * Initialise the buffer
 */
int init_buffer(struct buffer *buf, size_t sz, enum buffer_type type);

/* **
 * Adjust the buffer to the new size
 */
int realoc_buffer(struct buffer *buf, void *value, size_t sz);

/***
 * Destroy the buffer and free memory
 */
void destroy_buffer(struct buffer *buf);

/***
 * Set values
 */
int buffer_set_value(struct buffer *buf, const void *value, size_t sz);

#endif /* __BUFFER_H__ */
