
cmake_minimum_required(VERSION 3.10)

project(meta42)

set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include_directories(../sdk/include ../framework/src)

aux_source_directory(src SRC)
aux_source_directory(../framework/src FRAMEWORK)

add_executable(meta42-cli ${SRC} ${FRAMEWORK})
add_executable(meta42-test test/main.cpp)

target_link_libraries(meta42-cli ssl crypto)
target_link_libraries(meta42-test gtest pthread)

install(TARGETS meta42-cli DESTINATION bin)
install(TARGETS meta42-test DESTINATION bin)

# pack all files
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${deployment_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${deployment_VERSION_MINOR}")
include(CPack)