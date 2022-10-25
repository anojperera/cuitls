#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>
#include "buffer_types.h"

struct buffer {
        unsigned char init_flag;
        enum buffer_type type;
        size_t sz;
        void *buf;
};

/***
 * Initialise the buffer
 */
int init_buffer(struct buffer *buf, size_t sz, enum buffer_type type);

/* **
 * Adjust the buffer to the new size
 */
int realoc_buffer(struct buffer *buf, size_t sz);

/***
 * Destroy the buffer and free memory
 */
void destroy_buffer(struct buffer *buf);

/***
 * Set values
 */
int buffer_set_value(struct buffer *buf, const void *value, size_t sz);

#endif	/* __BUFFER_H__ */
