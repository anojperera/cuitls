/* This class for handling resource */
#ifndef __COLREF_H__
#define __COLREF_H__

#include "buffer.h"

struct colref {
  struct buffer resource;
  struct buffer colref;
};


/***
 * Initialise and destroy methods
 */
int colref_init(struct colref *obj, const char *res, const char *col);
void colref_free(struct colref *obj);

/***
 * Getters and setters
 */
const char * colref_get_resource(struct colref *obj);
const char * colref_get_colref(struct colref *obj);

#endif	/* __COLREF_H__ */
