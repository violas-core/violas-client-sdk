#
# IF(CMAKE_BUILD_TYPE MATCHES "Debug")
#     set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures_d.o)
#     link_directories(../../rust/violas-client/target/debug)
# ELSE()
#     set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)
#     link_directories(../../rust/violas-client/target/release)
# ENDIF()

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS -fcoroutines)

aux_source_directory(src SRC)

add_executable(nft-store ${SRC})

target_link_libraries(nft-store violas_sdk cpprest readline ssl crypto)

# test module
add_executable(test-nft-store test/main.cpp src/nft_store.cpp)
target_link_libraries(test-nft-store violas_sdk cpprest readline gtest pthread) 

# installation
install(TARGETS nft-store DESTINATION bin)
install(TARGETS test-nft-store DESTINATION bin)


