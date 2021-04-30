bin_PROGRAMS    = violas_cli

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/debug    
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/debug/build/violas-client-33336450db207f62/out/rust_cpp/cpp_closures.o
else
    # CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release    
    # RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release/build/violas-client-1ea0705af08078ac/out/rust_cpp/cpp_closures.o

    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/release    
    # file cpp_closures.o was compiled by rust
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/release/build/violas-client-609eda45df9365c2/out/rust_cpp/cpp_closures.o     
endif      

violas_cli_SOURCES = main.cpp  command.cpp
violas_cli_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/include -I$(top_srcdir)/rust/violas-client/src/ffi
violas_cli_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_cli_LDADD = -lviolas_client -lviolas_sdk -lreadline -lssl -lcrypto $(RUST_CPP_CLIENT) 