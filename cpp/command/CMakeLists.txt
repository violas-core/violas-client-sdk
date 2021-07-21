IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures_d.o)
    link_directories(../../rust/violas-client/target/debug)
ELSE() 
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)
    link_directories(../../rust/violas-client/target/release)
ENDIF()

aux_source_directory(src SRCS)

add_executable(cmd ${SRCS})

target_link_libraries(cmd violas_client violas_sdk cpprest readline pthread ssl crypto ${OBJ_FROM_RUST})

install(TARGETS cmd DESTINATION bin)