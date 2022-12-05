#include "../inc/url_hash.h"
#include "../inc/log.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int url_hash_init(struct url_hash *list)
{
        LOG_MESSAGE("url_hash iniialising");

        list->init_flag = CCSVCUBE_STATUS_SUCCESS;
        list->list = NULL;

        return CCSVCUBE_STATUS_SUCCESS;
}

void url_hash_delete(struct url_hash *list)
{
        struct url_hash_list *s, *tmp;
        if (list->init_flag != CCSVCUBE_STATUS_SUCCESS) {
                LOG_MESSAGE("url_hash not initialised");
                return;
        }

        if (!list->list) {
                LOG_MESSAGE("List is empty");
                return;
        }
        /* free the hash table contents */
        HASH_ITER(hh, list->list, s, tmp)
        {
                HASH_DEL(list->list, s);
                free(s);
        }
        list->list = NULL;
        LOG_MESSAGE("Deleted all contents of hash list");

        return;
}

int url_hash_add_item(struct url_hash *list, const char *method,
                      const char *path, void (*fn)(void *, void *))
{
        struct url_hash_list *s;
        if (list->init_flag != CCSVCUBE_STATUS_SUCCESS) {
                LOG_MESSAGE("List not initialised");
                return CCSVCUBE_STATUS_FAILED;
        }

        s = (struct url_hash_list *)malloc(sizeof(struct url_hash_list));
        s->fn = fn;
        memset(s->buf, 0, URL_HASH_BUF_SZ);

        snprintf(s->buf, URL_HASH_BUF_SZ, "%s-%s", method, path);
        HASH_ADD_KEYPTR(hh, list->list, s->buf, strlen(s->buf), s);

        return CCSVCUBE_STATUS_SUCCESS;
}

struct url_hash_list *url_hash_get_itme(struct url_hash *list,
                                        const char *method, const char *path)
{
        char tmp[URL_HASH_BUF_SZ];
        struct url_hash_list *s = NULL;
        if (list->init_flag != CCSVCUBE_STATUS_SUCCESS) {
                LOG_MESSAGE("List not initialised");
                return NULL;
        }

        memset(tmp, 0, URL_HASH_BUF_SZ);
        snprintf(tmp, URL_HASH_BUF_SZ, "%s-%s", method, path);
        HASH_FIND_STR(list->list, tmp, s);

        if (!s) {
                LOG_MESSAGE_ARGS("Unable to find item with key %s", tmp);
                return NULL;
        }

        return s;
}
