#
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures_d.o)
    link_directories(../../rust/violas-client/target/debug)
ELSE()
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)
    link_directories(../../rust/violas-client/target/release)
ENDIF()

aux_source_directory(src SRC)

add_executable(nft-store ${SRC})

target_link_libraries(nft-store violas_sdk violas_client cpprest readline ssl crypto ${OBJ_FROM_RUST})

install(TARGETS nft-store DESTINATION bin)



