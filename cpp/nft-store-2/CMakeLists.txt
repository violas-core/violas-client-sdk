cmake_minimum_required(VERSION 3.10)

project(nft-store-2 VERSION 1.0)

set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS -fcoroutines)
set(CMAKE_EXE_LINKER_FLAGS  -Wl,-rpath=./lib)
set(CMAKE_SHARED_LINKER_FLAGS -Wl,-rpath=./lib)

include_directories(../sdk/include ../framework/src 
                    ../../rust/violas-client/src/ffi)

aux_source_directory(../framework/src FRAMEWORK)

add_library(violas_framework SHARED ${FRAMEWORK} )

target_link_libraries(violas_framework)

add_library(violas_sdk SHARED ../sdk/src/violas_sdk2.cpp ../sdk/src/json_rpc.cpp ../sdk/src/console.cpp 
            ../sdk/src/ed25519 ../sdk/src/violas_client2.cpp ../sdk/src/wallet.cpp)

link_directories(../framework)

target_link_libraries(violas_sdk violas_framework)

aux_source_directory(src SRC)

add_executable(nft-store-2 ${SRC} ${FRAMEWORK} )

target_link_libraries(nft-store-2 violas_sdk cpprest readline ssl crypto)

# test module
add_executable(test-nft-store-2 test/main.cpp)
target_link_libraries(test-nft-store-2 violas_sdk cpprest readline gtest pthread) 

# installation
install(TARGETS nft-store-2 DESTINATION bin)
install(TARGETS test-nft-store-2 DESTINATION bin)
install(TARGETS violas_sdk DESTINATION lib)
install(TARGETS violas_framework DESTINATION lib)
install(DIRECTORY ../../move DESTINATION . FILES_MATCHING PATTERN "*.mv")