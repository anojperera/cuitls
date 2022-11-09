cmake_minimum_required(VERSION 3.20)
project(cutils LANGUAGES C)
set(CMAKE_C_STANDARD 99)
execute_process(COMMAND bash -c "mkdir -p build/json-c")
execute_process(COMMAND bash -c "cd build/json-c && sh ../../externals/json-c/cmake-configure --enable-static && make")
execute_process(COMMAND bash -c "cd build/json-c && cp -v ../../externals/json-c/*.h .")
execute_process(COMMAND bash -c "cd build && ln -s ../externals/uthash/include uthash")
add_executable(cutils
  src/main.c
  src/buffer.c
  src/colref.c
  src/geojson.c
  src/list.c
  src/mapping.c
  src/reader.c
  build/json-c/libjson-c.a
  )
target_include_directories(cutils PUBLIC
  inc
  externals
  build
  build/json-c
  )
target_link_directories(cutils PUBLIC
  build/json-c
  )
target_link_libraries(cutils PRIVATE curl)
target_link_libraries(cutils PRIVATE json-c.a)
