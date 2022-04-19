
cmake_minimum_required(VERSION 3.10)

project(meta42)

set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include_directories(../sdk/include ../framework/src)

aux_source_directory(src SRC)
aux_source_directory(../framework/src FRAMEWORK)
#aux_source_directory(../sdk/src SDK)

add_executable(meta42-cli ${SRC} ${FRAMEWORK})

add_executable(meta42-test test/main.cpp)

add_library(sdk STATIC  ../sdk/src/violas_client2.cpp 
                        ../sdk/src/console.cpp
                        ../sdk/src/ed25519.cpp
                        ../sdk/src/json_rpc.cpp
                        ../sdk/src/wallet.cpp)

target_link_libraries(meta42-cli sdk cpprest readline ssl crypto pthread)
target_link_libraries(meta42-test gtest pthread)

install(TARGETS meta42-cli DESTINATION bin)
install(TARGETS meta42-test DESTINATION bin)
install(FILES ../../move/build/modules/00_Meta42.mv DESTINATION ./move/modules/ )
install(FILES ../../move/build/package/stdlib/compiled/Compare.mv DESTINATION ./move/modules/ )
install(DIRECTORY ../../move/build/scripts DESTINATION ./move FILES_MATCHING PATTERN "meta42*.mv")

# pack all files
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${deployment_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${deployment_VERSION_MINOR}")
include(CPack)