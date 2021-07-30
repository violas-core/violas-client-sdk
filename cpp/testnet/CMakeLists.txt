IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures_d.o)
    link_directories(../../rust/violas-client/target/debug)
ELSE() 
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)
    link_directories(../../rust/violas-client/target/release)
ENDIF()

add_executable(testnet src/deployment.cpp)

target_link_libraries(testnet violas_client violas_sdk cpprest readline ${OBJ_FROM_RUST})

install(TARGETS testnet DESTINATION bin)