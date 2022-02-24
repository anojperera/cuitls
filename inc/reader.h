#ifndef __READER_H__
#define __READER_H__

#include "buffer.h"
#include "json-c/json.h"

int read_file(const char *file_path, struct buffer *buf);
int read_json_file(const char *file_path, struct buffer *buf, json_object **obj);
#endif	/* __READER_H__ */
