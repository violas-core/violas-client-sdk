cmake_minimum_required(VERSION 3.10)

project(violas-client-sdk VERSION 1.0)

set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
#set(CMAKE_CXX_FLAGS -Werror)
set(CMAKE_EXE_LINKER_FLAGS -Wl,-rpath=./lib)
set(CMAKE_SHARED_LINKER_FLAGS -Wl,-rpath=./lib)

include_directories(sdk/include framework/src ../rust/violas-client/src/ffi)
link_directories(../../rust/violas-client/target/debug)

add_subdirectory(framework)

# library violas_client 
add_subdirectory(sdk)

# executable nft
# add_subdirectory(nft)

# add_subdirectory(nft-store)

# add_subdirectory(nft-store-2)

# executable cmd
# add_subdirectory(command)

# add_subdirectory(testnet)

# executable vls
# add_subdirectory(vls)

install(DIRECTORY ../move DESTINATION . FILES_MATCHING PATTERN "*.mv")

# install mnemonic files 
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    install(DIRECTORY testnet/mnemonic DESTINATION .)
    install(PROGRAMS ../rust/violas-oracle/target/debug/violas-oracle TYPE BIN)
ELSE()
    #install(FILES testnet/mnemonic/nft.mne DESTINATION mnemonic)
    install(FILES /lib/x86_64-linux-gnu/libcpprest.so.2.10 DESTINATION lib)
    install(PROGRAMS ../rust/violas-oracle/target/release/violas-oracle TYPE BIN)
ENDIF()

# pack all files
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${deployment_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${deployment_VERSION_MINOR}")
include(CPack)

# Run cmake with release as below
# cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -DCMAKE_EXE_LINKER_FLAGS="-Wl,-rpath=./lib" -DCMAKE_CXX_FLAGS="-Werror -Wall"