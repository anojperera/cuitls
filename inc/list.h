/* This is a class to handle lists */

#ifdef __LIST_H__
#define __LIST_H__

#include <stdlib.h>

#include "buffer.h"

struct list {
  unsigned int init_flg;
  struct buffer buf;
  struct list* prev;
  struct list* next;
};


/*
 * Initialise and Delete Methods
 */
int list_init(struct list* list);
void list_delete(struct list* list);


/*
 * Add and remove Methods
 */
int list_add_item(struct list* list, void* obj);
int list_remove_item(struct list* list, void *obj);

/*
 * Get items
 */
const struct list* list_get_item(struct list *list, unsigned int ix);
unsigned int list_get_count(struct list* list);


#endif    /* __LIST_H__ */
