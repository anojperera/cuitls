#include "../../inc/buffer_types.h"
#include "../inc/buffer.h"
#include "../inc/geojson.h"
#include "../inc/log.h"
#include "../inc/reader.h"
#include "../inc/status.h"
#include "json-c/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PATH "./pipeswithcriticandcapa.geojson"
int geo_main(int argc, char **argv);
int web_main(int argc, char **argv);

int main(int argc, char **argv) { return geo_main(argc, argv); }

int geo_main(int argc, char **argv) {
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

int web_main(int argc, char **argv) {
  char *uri = argv[1];

  LOG_MESSAGE("Starting WEB URI Program");

  return CCSVCUBE_STATUS_SUCCESS;
}
