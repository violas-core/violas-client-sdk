cmake_minimum_required(VERSION 3.0)

project(violas_cli VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SRCS)

include_directories(../../rust/violas-client/src/ffi)

# add_library(rust_obj
#         STATIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/../../rust/violas-client/target/release/build/violas-client-c45e8aab37bfb9f5/out/rust_cpp/violas_client.o)

set(OBJS
  ${CMAKE_CURRENT_SOURCE_DIR}/../../rust/violas-client/target/release/build/violas-client-c45e8aab37bfb9f5/out/rust_cpp/cpp_closures.o
)

SET_SOURCE_FILES_PROPERTIES(
  ${OBJS}
  PROPERTIES
  EXTERNAL_OBJECT true
  GENERATED true
)

add_definitions(${CMAKE_CURRENT_SOURCE_DIR}/../../rust/violas-client/target/release/build/violas-client-c45e8aab37bfb9f5/out/rust_cpp/cpp_closures.o)

add_link_options(${CMAKE_CURRENT_SOURCE_DIR}/../../rust/violas-client/target/release/build/violas-client-c45e8aab37bfb9f5/out/rust_cpp/cpp_closures.o)

link_directories(../../rust/violas-client/target/release/build/violas-client-c45e8aab37bfb9f5/out/rust_cpp/)

add_executable(violas_cli ${SRCS})

target_link_libraries(violas_cli readline pthread ${OBJS})
