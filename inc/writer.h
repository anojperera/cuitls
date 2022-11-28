/* Functionality for writing files */
#ifndef __WRITER_H__
#define __WRITER_H__

#include "buffer.h"
#include "buffer_types.h"

/*
 * Writing buffer to file blocking
 */
int writer_write_file_blocking(struct buffer *buf, const char *fp);

#endif /* __WRITER_H__ */
