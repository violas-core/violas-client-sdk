#include_directories(/usr/lib/jvm/java-11-openjdk-amd64/include /usr/lib/jvm/java-11-openjdk-amd64/include/linux)
#aux_source_directory(src SRC)
#add_library(violas_sdk SHARED ${SRC} )

add_library(violas_sdk SHARED src/violas_sdk2.cpp src/json_rpc.cpp src/console.cpp src/nft.cpp)

# target_link_libraries(violas_client)

install(TARGETS violas_sdk DESTINATION lib)
install(FILES ../../rust/violas-client/target/release/libviolas_client.so DESTINATION lib)
install(FILES include/violas_sdk2.hpp DESTINATION include)
