#include "../../inc/buffer_types.h"
#include "../inc/buffer.h"
#include "../inc/geojson.h"
#include "../inc/log.h"
#include "../inc/reader.h"
#include "../inc/status.h"
#include "../inc/url_hash.h"
#include "../inc/writer.h"
#include "http.h"
#include "json-c/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PATH "./pipeswithcriticandcapa.geojson"
void geo_main(void *req, void *obj);
void web_main(void *req, void *obj);

// We'll use this callback in `http_listen`, to handles HTTP requests
void on_request(http_s *request);

// These will contain pre-allocated values that we will use often
FIOBJ HTTP_X_DATA;

// Listen to HTTP requests and start facil.io
int main(int argc, char const **argv)
{
        /*  initialise the hash map */
        struct url_hash url_list;
        // allocating values we use often
        HTTP_X_DATA = fiobj_str_new("X-Data", 6);

        LOG_MESSAGE("Initialising the HASH and adding endpoint handlers");
        url_hash_init(&url_list);
        url_hash_add_item(&url_list, "POST", "/external", web_main);
        url_hash_add_item(&url_list, "POST", "/geo/split", geo_main);

        // listen on port 3000 and any available network binding (NULL ==
        // 0.0.0.0)
        http_listen("3000", NULL, .on_request = on_request, .log = 1,
                    .udata = &url_list);

        fio_start(.threads = 1);

        // deallocating the common values
        fiobj_free(HTTP_X_DATA);
        url_hash_delete(&url_list);

        return CCSVCUBE_STATUS_SUCCESS;
}

// Easy HTTP handling
void on_request(http_s *request)
{
        FIOBJ method = request->method;
        fio_str_info_s method_s = fiobj_obj2cstr(request->method);
        fio_str_info_s path_s = fiobj_obj2cstr(request->path);
        struct url_hash *list = (struct url_hash *)request->udata;
        struct url_hash_list *h = NULL;

        /*  Pass the request and the path to hash list to find a method attached
         */
        if (method_s.len > 0 && path_s.len > 0) {
                LOG_MESSAGE_ARGS(
                    "Checking method attached to HTTP Method %s and path %s",
                    method_s.data, path_s.data);
                h = url_hash_get_itme(list, method_s.data, path_s.data);
        }

        if (h) {
                LOG_MESSAGE("Method found");
                h->fn(request, NULL);
        }

        http_set_cookie(request, .name = "my_cookie", .name_len = 9,
                        .value = "data", .value_len = 4);
        http_set_header(request, HTTP_HEADER_CONTENT_TYPE,
                        http_mimetype_find("txt", 3));
        http_set_header(request, HTTP_X_DATA, fiobj_str_new("my data", 7));
        http_send_body(request, "Hello World!\r\n", 14);
}

void geo_main(void *req, void *obj)
{
        struct buffer buf;
        const char tmp_str[] = "This is a string";
        json_object *jobj = NULL;
        int status = CCSVCUBE_STATUS_SUCCESS;
        struct geojson geo;
        char *fp, *bf_v;
        http_s *request = (http_s *)req;
        fio_str_info_s body;
        struct buffer b;
        json_object *json = NULL;
        json_object *file = NULL;
        json_object *bf = NULL;
        json_object *pz = NULL;

        json_bool json_stat;
        enum json_type jtype;

        if (!request->body) {
                LOG_MESSAGE("Body is missing in the reuqest");
                return;
        }

        body = fiobj_obj2cstr(request->body);
        if (body.len == 0) {
                LOG_MESSAGE("Body lenght is zeor");
                return;
        }
        init_buffer(&b, body.len, buffer_type_unknown);
        buffer_set_value(&b, body.data, body.len);
        status = read_json_from_buf(&b, &json);
        jtype = json_object_get_type(json);
        if (jtype != json_type_object) {
                LOG_MESSAGE("Body expects a json object at root");
                return;
        }

        json_stat = json_object_object_get_ex(json, "file", &file);
        if (!json_stat) {
                LOG_MESSAGE("Unable to get the name from the json body");
                return;
        }

        fp = json_object_get_string(file);
        if (fp == NULL) {
                LOG_MESSAGE("Error getting name of the file");
        } else {
                LOG_MESSAGE_ARGS("File name for the download: %s", fp);
        }

        json_stat = json_object_object_get_ex(json, "baseFile", &bf);
        if (!json_stat) {
                LOG_MESSAGE("Unable to get the URL from json body");
                return;
        }

        bf_v = json_object_get_string(bf);
        if (bf_v == NULL) {
                LOG_MESSAGE("Unable to get the uri from json object");
        } else {
                LOG_MESSAGE_ARGS("URI %s", bf_v);
        }

        LOG_MESSAGE("Testing JSON reader");
        if (read_json_file(fp, &buf, &jobj) == CCSVCUBE_STATUS_FAILED) {
                LOG_MESSAGE("Failed to parse json");
                destroy_buffer(&buf);
                return;
        }

        LOG_MESSAGE("Initialising geojson object");
        geojson_init(&geo);

        geo.base_file = bf_v;

        json_stat = json_object_object_get_ex(json, "baseFile", &pz);
        if (!json_stat) {
                LOG_MESSAGE("Unable to get the URL from json body");
        } else {
                geo.page_sz = json_object_get_int(pz);
        }

        LOG_MESSAGE("Splitting into multiple geojson files");
        geojson_split_into_multiples(&geo, jobj);

        geojson_free(&geo);

        LOG_MESSAGE("Complete Test");

        destroy_buffer(&buf);
}

void web_main(void *req, void *obj)
{
        struct buffer buf;
        struct buffer b;
        int status = CCSVCUBE_STATUS_SUCCESS;
        const char *uri;
        const char *fp;
        http_s *request = (http_s *)req;
        json_object *json = NULL;
        json_object *name = NULL;
        json_object *url = NULL;
        json_bool json_stat;
        enum json_type jtype;
        fio_str_info_s body;

        if (!request->body) {
                LOG_MESSAGE("Body is missing in the reuqest");
                return;
        }

        body = fiobj_obj2cstr(request->body);
        if (body.len == 0) {
                LOG_MESSAGE("Body lenght is zeor");
                return;
        }
        init_buffer(&b, body.len, buffer_type_unknown);
        buffer_set_value(&b, body.data, body.len);
        status = read_json_from_buf(&b, &json);

        if (status != CCSVCUBE_STATUS_SUCCESS) {
                LOG_MESSAGE("Unable to parse json");
                return;
        }

        LOG_MESSAGE("Parsing json body");

        jtype = json_object_get_type(json);
        if (jtype != json_type_object) {
                LOG_MESSAGE("Body expects a json object at root");
                return;
        }

        json_stat = json_object_object_get_ex(json, "name", &name);
        if (!json_stat) {
                LOG_MESSAGE("Unable to get the name from the json body");
                return;
        }

        fp = json_object_get_string(name);
        if (fp == NULL) {
                LOG_MESSAGE("Error getting name of the file");
        } else {
                LOG_MESSAGE_ARGS("File name for the download: %s", fp);
        }

        json_stat = json_object_object_get_ex(json, "url", &url);
        if (!json_stat) {
                LOG_MESSAGE("Unable to get the URL from json body");
                return;
        }

        uri = json_object_get_string(url);
        if (uri == NULL) {
                LOG_MESSAGE("Unable to get the uri from json object");
        } else {
                LOG_MESSAGE_ARGS("URI %s", uri);
        }

        init_buffer(&buf, 0, buffer_type_unknown);
        status = read_remote_uri(uri, &buf);

        if (status == CCSVCUBE_STATUS_FAILED) {
                LOG_MESSAGE("Unable to read the URL");
                goto ERROR_HANDLER;
        }

        /* Write to the disk */
        LOG_MESSAGE_ARGS("Writing data to file: %s", fp);
        status = writer_write_file_blocking(&buf, fp);

        if (status != CCSVCUBE_STATUS_SUCCESS) {
                LOG_MESSAGE("Unable to write file");
        }

ERROR_HANDLER:
        LOG_MESSAGE("Destroying buffer and cleaning up");
        destroy_buffer(&buf);
}
