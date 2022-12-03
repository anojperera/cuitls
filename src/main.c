#include "../../inc/buffer_types.h"
#include "../inc/buffer.h"
#include "../inc/geojson.h"
#include "../inc/log.h"
#include "../inc/reader.h"
#include "../inc/status.h"
#include "../inc/writer.h"
#include "http.h"
#include "json-c/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PATH "./pipeswithcriticandcapa.geojson"
int geo_main(int argc, char **argv);
int web_main(int argc, char **argv);
// We'll use this callback in `http_listen`, to handles HTTP requests
void on_request(http_s *request);

// These will contain pre-allocated values that we will use often
FIOBJ HTTP_X_DATA;

// Listen to HTTP requests and start facil.io
int main(int argc, char const **argv)
{
        // allocating values we use often
        HTTP_X_DATA = fiobj_str_new("X-Data", 6);
        // listen on port 3000 and any available network binding (NULL ==
        // 0.0.0.0)
        http_listen("3000", NULL, .on_request = on_request, .log = 1);

        fio_start(.threads = 1);

        // deallocating the common values
        fiobj_free(HTTP_X_DATA);
}

// Easy HTTP handling
void on_request(http_s *request)
{
        http_set_cookie(request, .name = "my_cookie", .name_len = 9,
                        .value = "data", .value_len = 4);
        http_set_header(request, HTTP_HEADER_CONTENT_TYPE,
                        http_mimetype_find("txt", 3));
        http_set_header(request, HTTP_X_DATA, fiobj_str_new("my data", 7));
        http_send_body(request, "Hello World!\r\n", 14);
}

int geo_main(int argc, char **argv)
{
        struct buffer buf;
        const char tmp_str[] = "This is a string";
        json_object *jobj = NULL;
        struct geojson geo;

        LOG_MESSAGE_ARGS("Number of arguments pased: %i", argc);
        if (argc < 2) {
                LOG_MESSAGE("Please pass the file name as an argument");
                return CCSVCUBE_STATUS_FAILED;
        }

        LOG_MESSAGE("Starting test program");
        /* initialise the buffer  */

        LOG_MESSAGE("Testing buffer");
        init_buffer(&buf, 20, buffer_type_str);
        buffer_set_value(&buf, tmp_str, sizeof(tmp_str));

        destroy_buffer(&buf);

        LOG_MESSAGE("Testing JSON reader");
        if (read_json_file(argv[1], &buf, &jobj) == CCSVCUBE_STATUS_FAILED) {
                LOG_MESSAGE("Failed to parse json");
                destroy_buffer(&buf);
                return CCSVCUBE_STATUS_FAILED;
        }

        LOG_MESSAGE("Initialising geojson object");
        geojson_init(&geo);

        if (argc > 2) {
                geo.base_file = argv[2];
        }

        if (argc > 3) {
                geo.page_sz = atoi(argv[3]);
        }

        LOG_MESSAGE("Splitting into multiple geojson files");
        geojson_split_into_multiples(&geo, jobj);

        geojson_free(&geo);

        LOG_MESSAGE("Complete Test");

        destroy_buffer(&buf);
        return CCSVCUBE_STATUS_SUCCESS;
}

int web_main(int argc, char **argv)
{
        struct buffer buf;
        int status = CCSVCUBE_STATUS_SUCCESS;
        char *uri, *fp;

        if (argc < 3) {
                LOG_MESSAGE("Required arguments is not provided");
                return CCSVCUBE_STATUS_FAILED;
        }

        uri = argv[1];
        fp = argv[2];

        LOG_MESSAGE_ARGS("Starting web parser for %s", uri);

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

        return status;
}
