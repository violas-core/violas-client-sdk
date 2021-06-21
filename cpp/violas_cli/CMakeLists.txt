set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)

link_directories(../../rust/violas-client/target/release)

aux_source_directory(src SRCS)

add_executable(cli ${SRCS})

target_link_libraries(cli violas_client violas_sdk readline pthread ssl crypto ${OBJ_FROM_RUST})

install(TARGETS cli DESTINATION bin)