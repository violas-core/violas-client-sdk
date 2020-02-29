# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        violas-sdk-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        client.cpp
        io_violas_sdk_Client.cpp
        io_violas_sdk_java_Client.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

include_directories("../../../../../cppSdk/src/")
include_directories(${CMAKE_SOURCE_DIR}/../../../../../cppSdk/src)

add_library(c++fs STATIC IMPORTED)
set_property(TARGET c++fs PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../../../../cppSdk/lib/libc++fs-arm64v8a.a)

add_library(cppSdk STATIC IMPORTED)
set_property(TARGET cppSdk PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../../../../cppSdk/lib/libcppSdk-arm64v8a.a)

add_library(client_proxy STATIC IMPORTED)
set_property(TARGET client_proxy PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../../../../rust-client-proxy/target/aarch64-linux-android/debug/libclient_proxy.a)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        violas-sdk-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        cppSdk
        c++fs
        client_proxy)