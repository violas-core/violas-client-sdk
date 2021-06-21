cmake_minimum_required(VERSION 3.10)

project(deployment VERSION 1.0)

set(CMAKE_CXX_COMPILER clang++-12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include_directories(./include ../rust/violas-client/src/ffi)

# library violas_client 
add_subdirectory(src)

# executable deployment
add_subdirectory(deployment)

# executable cli
add_subdirectory(violas_cli)

# pack all files
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${deployment_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${deployment_VERSION_MINOR}")
include(CPack)