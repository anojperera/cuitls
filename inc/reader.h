#ifndef __READER_H__
#define __READER_H__

#include "buffer.h"

int read_file(const char *file_path, struct buffer *buf);

#endif	/* __READER_H__ */
