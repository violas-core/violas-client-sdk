cmake_minimum_required(VERSION 3.0)

project (violas-client VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

aux_source_directory(src SRCS)

add_executable(violas-client ${SRCS})

target_link_libraries(violas-client ssl crypto)