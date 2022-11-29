#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../inc/buffer.h"
#include "../inc/buffer_types.h"
#include "../inc/log.h"
#include "../inc/status.h"
#include "json-c/json.h"
#include "json-c/json_tokener.h"
#include "json-c/json_types.h"

#include <curl/curl.h>

size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata);

int read_file(const char *file_path, struct buffer *buf)
{
        int fd;
        off_t buf_sz;
        size_t read_bytes;

        LOG_MESSAGE("Getting file descriptor, opening generic file");
        fd = open(file_path, O_RDONLY);
        if (fd == CCSVCUBE_STATUS_FAILED) {
                /* File failed to open, get the error code and report to user */
                LOG_MESSAGE_ARGS("%s\n", strerror(errno));
                goto ERROR_HANDLER;
        }

        /* set the file pointer to zero */
        lseek(fd, 0, SEEK_SET);
        buf_sz = lseek(fd, 0, SEEK_END);

        if (buf_sz == 0) {
                LOG_MESSAGE("File size is zero");
                goto ERROR_HANDLER;
        }

        /* Reset and read the bites */
        lseek(fd, 0, SEEK_SET);
        init_buffer(buf, buf_sz + 1, buffer_type_str);

        read_bytes = read(fd, buf->buf, buf_sz);
        if (read_bytes < buf_sz) {
                LOG_MESSAGE_ARGS(
                    "Number of bytes read (%ul) does not match expected\n",
                    read_bytes);
                goto ERROR_HANDLER;
        }

        /* Set additional attributes */
        buf->sz = buf_sz;
        buf->type = buffer_type_str;

        LOG_MESSAGE_ARGS("File %s read successfully", file_path);
        close(fd);
        return CCSVCUBE_STATUS_SUCCESS;

ERROR_HANDLER:
        if (fd != CCSVCUBE_STATUS_FAILED) {
                close(fd);
        }

        return CCSVCUBE_STATUS_FAILED;
}

int read_json_file(const char *file_path, struct buffer *buf, json_object **obj)
{
        struct json_tokener *tok = NULL;
        enum json_tokener_error jerr;

        LOG_MESSAGE("Opening JSON file");
        if (read_file(file_path, buf) == CCSVCUBE_STATUS_FAILED) {
                LOG_MESSAGE_ARGS("Errors occured while reading %s", file_path);
                goto ERROR_HANDLER;
        }

        LOG_MESSAGE("Creating JSON tokener");
        tok = json_tokener_new();

        if (tok == NULL) {
                LOG_MESSAGE("Error creating json_tokener");
                goto ERROR_HANDLER;
        }

        do {
                *obj = json_tokener_parse_ex(tok, buf->buf, buf->sz);
        } while ((jerr = json_tokener_get_error(tok)) == json_tokener_continue);

        if (jerr != json_tokener_success) {
                LOG_MESSAGE_ARGS("Errors occured: %s",
                                 json_tokener_error_desc(jerr));
                json_tokener_free(tok);

                goto ERROR_HANDLER;
        }

        if (json_tokener_get_parse_end(tok) < buf->sz) {
                LOG_MESSAGE("JSON tokeniser parsing of file was not complete");
        }

        LOG_MESSAGE("Freeing tokeniser");
        json_tokener_free(tok);

        return CCSVCUBE_STATUS_SUCCESS;

ERROR_HANDLER:
        return CCSVCUBE_STATUS_FAILED;
}

int read_remote_uri(const char *uri, struct buffer *buf)
{
        CURL *curl;
        char curl_errbuf[CURL_ERROR_SIZE];
        int err;

        LOG_MESSAGE_ARGS("Getting data from %s", uri);
        LOG_MESSAGE("Initialising CURL");

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, uri);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, buf);

        err = curl_easy_perform(curl);

        if (!err) {
                LOG_MESSAGE("CURL successfully downloaded the data");
        } else {
                LOG_MESSAGE_ARGS("Following error's occured while downloading "
                                 "data from %s: %",
                                 uri, curl_errbuf);
        }

        curl_easy_cleanup(curl);

        return CCSVCUBE_STATUS_SUCCESS;
}

size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
        struct buffer *buf = NULL;
        size_t realsize = size * nmemb;
        int stat = CCSVCUBE_STATUS_SUCCESS;

        if (!userdata) {
                LOG_MESSAGE("Userdata is not passed to the method");
                return -1;
        }
        buf = (struct buffer *)userdata;

        stat = realoc_buffer(buf, ptr, realsize);
        if (stat != CCSVCUBE_STATUS_SUCCESS) {
                return stat;
        }

        return realsize;
}
