#ifndef __READER_H__
#define __READER_H__

#include "buffer.h"
#include <json-c/json.h>

int read_file(const char *file_path, struct buffer *buf);
int read_json_file(const char *file_path, struct buffer *buf,
                   json_object **obj);
int read_json_from_buf(struct buffer *buf, json_object **obj);

/***
 * Read content from a remote resource
 */
int read_remote_uri(const char *uri, struct buffer *buf);

#endif /* __READER_H__ */
