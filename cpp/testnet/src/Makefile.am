bin_PROGRAMS    = violas_client violas_vls violas_swap

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/debug    
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/debug/build/violas-client-18f7e29f0bd720d5/out/rust_cpp/cpp_closures.o
else
    # CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release    
    # RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release/build/violas-client-1ea0705af08078ac/out/rust_cpp/cpp_closures.o

    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/release    
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/release/build/violas-client-c796d9c49274bb62/out/rust_cpp/cpp_closures.o     
endif      

violas_client_SOURCES = violas_client.cpp 
violas_client_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/violas-client/src/ffi
violas_client_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_client_LDADD = -lviolas_client -lviolas_sdk $(RUST_CPP_CLIENT) 

violas_vls_SOURCES = violas_vls.cpp 
violas_vls_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/violas-client/src/ffi
violas_vls_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_vls_LDADD = -lviolas_client -lviolas_sdk $(RUST_CPP_CLIENT)

violas_swap_SOURCES = violas_swap.cpp 
violas_swap_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/violas-client/src/ffi
violas_swap_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_swap_LDADD = -lviolas_client -lviolas_sdk $(RUST_CPP_CLIENT)
