/***
 * This is a list for handling hashed list of method and path and fn pointer
 */
#ifndef __URL_HASH_H__
#define __URL_HASH_H__

#include "status.h"
#include "uthash/uthash.h"
#define URL_HASH_BUF_SZ 256

struct url_hash_list {
        char buf[URL_HASH_BUF_SZ];
        void (*fn)(void *, void *);
        UT_hash_handle hh;
};

struct url_hash {
        int init_flag;
        struct url_hash_list *list;
};

/*  Initialise and delete the list */
int url_hash_init(struct url_hash *list);
void url_hash_delete(struct url_hash *list);

/*  Add and get item */
int url_hash_add_item(struct url_hash *list, const char *method,
                      const char *path, void (*fn)(void *, void *));
struct url_hash_list *url_hash_get_itme(struct url_hash *list,
                                        const char *method, const char *path);

#endif /*  __URL_HASH_H__ */
