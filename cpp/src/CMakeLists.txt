add_library(violas_sdk violas_sdk2.cpp json_rpc.cpp)

# target_link_libraries(violas_client)

install(TARGETS violas_sdk DESTINATION lib)
install(FILES ../../rust/violas-client/target/release/libviolas_client.so DESTINATION lib)
install(FILES ../include/violas_sdk2.hpp DESTINATION include)
