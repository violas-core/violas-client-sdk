set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS -fcoroutines)
set(CMAKE_EXE_LINKER_FLAGS  -Wl,-rpath=./lib)

aux_source_directory(src SRC)

add_executable(nft-store-2 ${SRC})

target_link_libraries(nft-store-2 violas_sdk cpprest readline ssl crypto)

# test module
add_executable(test-nft-store-2 test/main.cpp)
target_link_libraries(test-nft-store-2 violas_sdk cpprest readline gtest pthread) 

# installation
install(TARGETS nft-store-2 DESTINATION bin)
install(TARGETS test-nft-store-2 DESTINATION bin)