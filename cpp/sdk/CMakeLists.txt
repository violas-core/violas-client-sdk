#include_directories(/usr/lib/jvm/java-11-openjdk-amd64/include /usr/lib/jvm/java-11-openjdk-amd64/include/linux)
#aux_source_directory(src SRC)
#add_library(violas_sdk SHARED ${SRC} )

add_library(violas_sdk SHARED src/violas_sdk2.cpp src/json_rpc.cpp src/console.cpp src/ed25519 src/violas_client2.cpp)

# target_link_libraries(violas_client)

install(TARGETS violas_sdk DESTINATION lib)
install(FILES include/violas_sdk2.hpp DESTINATION include)
install(FILES include/nft.hpp DESTINATION include)
install(FILES include/nft.cpp DESTINATION include)
#install(FILES ../../rust/violas-client/target/release/libviolas_client.so DESTINATION lib)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    install(FILES ../../rust/violas-client/target/debug/libviolas_client.so DESTINATION lib)
ELSE()
    install(FILES ../../rust/violas-client/target/release/libviolas_client.so DESTINATION lib)
ENDIF()