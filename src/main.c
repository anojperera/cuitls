#include <stdlib.h>
#include <stdio.h>
#include "../inc/buffer_types.h"
#include "../inc/buffer.h"
#include "../inc/reader.h"
#include "../inc/log.h"
#include "../inc/status.h"
#include "json-c/json.h"

#define FILE_PATH "./pipeswithcriticandcapa.geojson"

int main(int argc, char** argv)
{
        struct buffer buf;
        const char tmp_str[] = "This is a string";
        json_object *jobj = NULL;

        LOG_MESSAGE("Starting test program");
        /* initialise the buffer  */

        LOG_MESSAGE("Testing buffer");
        init_buffer(&buf, 20, buffer_type_str);
        buffer_set_value(&buf, tmp_str, sizeof(tmp_str));

        destroy_buffer(&buf);

        /* Read the file and show the contents */
        LOG_MESSAGE("Testing file reader");
        if (read_file(FILE_PATH, &buf) == CCSVCUBE_STATUS_SUCCESS) {
                fprintf(stdout, "Buffer contents %s\n", buf.buf);
                destroy_buffer(&buf);
        }

        LOG_MESSAGE("Testing JSON reader");
        if (read_json_file(FILE_PATH, &buf, &jobj) == CCSVCUBE_STATUS_FAILED) {
                LOG_MESSAGE("Failed to parse json");
                destroy_buffer(&buf);
                return CCSVCUBE_STATUS_FAILED;
        }



        LOG_MESSAGE("Complete Test");

        return CCSVCUBE_STATUS_SUCCESS;
}
