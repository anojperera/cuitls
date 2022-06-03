/* This is a class to handle lists */

#ifdef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include "uthash/utarray.h"
#include "buffer.h"

struct list {
  unsigned int init_flg;
  UT_array* arr;
  size_t count;
};

/*
 * Initialise and Delete Methods
 */
int list_init(struct list* list);
void list_delete(struct list* list);


/*
 * Add and remove Methods
 */
int list_add_item(struct list* list, void* obj, size_t sz);
int list_remove_item(struct list* list, void *obj);

/*
 * Get items
 */
void* list_get_item(struct list* list, size_t ix);
size_t list_get_item_count(struct list* list);

#endif    /* __LIST_H__ */
