cmake_minimum_required(VERSION 3.20)
project(cutils LANGUAGES C)
set(CMAKE_C_STANDARD 99)
# execute_process(COMMAND bash -c "mkdir -p build/json-c")
# execute_process(COMMAND bash -c "cd build/json-c && sh ../../externals/json-c/cmake-configure --enable-static && make")
# execute_process(COMMAND bash -c "cd build/json-c && cp -v ../../externals/json-c/*.h .")
execute_process(COMMAND bash -c "cd build && ln -s ../externals/uthash/include uthash")
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/build)
include_directories(${PROJECT_SOURCE_DIR}/externals)

include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io)
include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io/lib/facil)
include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io/lib/facil/fiobj)
include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io/lib/facil/http)
include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io/lib/facil/redis)
include_directories(${PROJECT_SOURCE_DIR}/externals/facil.io/lib/facil/tls)
include_directories(${PROJECT_SOURCE_DIR}/externals/json-c)


add_subdirectory(externals/facil.io)
add_subdirectory(externals/json-c)

add_executable(cutils
  src/main.c
  src/buffer.c
  src/colref.c
  src/geojson.c
  src/list.c
  src/mapping.c
  src/reader.c
  src/writer.c
  src/url_hash.c
  )
find_package(json-c CONFIG)
target_link_libraries(cutils PRIVATE curl)
target_link_libraries(cutils PRIVATE json-c)
target_link_libraries(cutils PRIVATE facil.io)
