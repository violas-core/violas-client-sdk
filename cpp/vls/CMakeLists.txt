IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../../rust/violas-client/target/debug/build/violas-client-e89d173348ec1c91/out/rust_cpp/cpp_closures.o)
    link_directories(../../rust/violas-client/target/debug)
ELSE() 
    set(OBJ_FROM_RUST ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cpp_closures.o)
    link_directories(../../rust/violas-client/target/release)
ENDIF()

add_executable(vls src/vls.cpp)

target_link_libraries(vls violas_client violas_sdk cpprest readline ${OBJ_FROM_RUST})

install(TARGETS vls DESTINATION bin)