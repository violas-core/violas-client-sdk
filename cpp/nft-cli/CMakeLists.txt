cmake_minimum_required(VERSION 3.10)

project(nft)

set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

aux_source_directory(src SRC)
aux_source_directory(../framework/src FRAMEWORK)

include_directories(../sdk/include ../framework/src)

add_library(sdk STATIC  ../sdk/src/violas_client2.cpp 
                        ../sdk/src/console.cpp
                        ../sdk/src/ed25519.cpp
                        ../sdk/src/json_rpc.cpp
                        ../sdk/src/wallet.cpp)

add_executable(nft-cli ${SRC} ${FRAMEWORK})
add_executable(nft-test test/main.cpp) 

target_link_libraries(nft-cli sdk cpprest readline sqlite3 ssl crypto pthread dl)
target_link_libraries(nft-test gtest pthread)

install(TARGETS nft-cli DESTINATION bin)
install(TARGETS nft-test DESTINATION bin)
install(FILES ../../move/build/modules/00_Meta42.mv DESTINATION ./move/modules/ )
install(FILES ../../move/build/package/stdlib/compiled/Compare.mv DESTINATION ./move/modules/ )
install(DIRECTORY ../../move/build/scripts DESTINATION ./move FILES_MATCHING PATTERN "nft*.mv")
install(FILES ../testnet/mnemonic/nft.mne DESTINATION ./mnemonic )

# pack all files
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${deployment_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${deployment_VERSION_MINOR}")
include(CPack)