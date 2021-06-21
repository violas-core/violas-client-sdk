cmake_minimum_required(VERSION 3.10.0)

project(test_bcs)

set(CMAKE_CXX_STANDARD 20)

include_directories(../include)

add_executable(test_bcs test_bcs.cpp)


