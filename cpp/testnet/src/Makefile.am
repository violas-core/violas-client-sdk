bin_PROGRAMS    = violas_client violas_vls

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/debug
    CLIENT_PROXY = -lviolas_client
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/debug/build/violas-client-18f7e29f0bd720d5/out/rust_cpp/cpp_closures.o
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/release
    CLIENT_PROXY =  -lviolas_client
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/release/build/violas-client-e492e5f30c5b3c32/out/rust_cpp/cpp_closures.o
endif

violas_client_SOURCES = violas_client.cpp 
violas_client_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/violas-client/src/ffi
violas_client_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_client_LDADD = $(CLIENT_PROXY) $(RUST_CPP_CLIENT) -lviolas_sdk

violas_vls_SOURCES = violas_vls.cpp 
violas_vls_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/violas-client/src/ffi
violas_vls_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_vls_LDADD = $(CLIENT_PROXY) $(RUST_CPP_CLIENT) -lviolas_sdk

