bin_PROGRAMS    = testnet test_jni transfer violas_client

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/debug
    CLIENT_PROXY = -lclient_proxy 
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/release
    CLIENT_PROXY =  -lclient_proxy
endif

testnet_SOURCES = test_violas.cpp test_libra.cpp test_exchange.cpp test_bank.cpp main.cpp
testnet_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/client-proxy/src/ffi
testnet_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L$(top_builddir)/cpp/src/.libs $(CLIENT_PROXY_PATH) 
testnet_LDADD = -lviolas_sdk -llibra_sdk $(CLIENT_PROXY) -lstdc++fs

test_jni_SOURCES = test_violas.cpp test_libra.cpp test_exchange.cpp test_bank.cpp main.cpp
test_jni_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/client-proxy/src/ffi
test_jni_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L../../src/.libs -L../../../lib $(CLIENT_PROXY_PATH)                  
test_jni_LDADD = -lviolas_sdk_jni -lviolas_sdk -llibra_sdk $(CLIENT_PROXY) -lstdc++fs

transfer_SOURCES = transfer.cpp
transfer_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/client-proxy/src/ffi
transfer_LDFLAGS = -Wl,-rpath=./bin,-rpath=../bin,-rpath=/usr/local/lib \
                  -L../../src/.libs $(CLIENT_PROXY_PATH) 
transfer_LDADD = -lviolas_sdk $(CLIENT_PROXY) -lstdc++fs

# file cpp_closures.o was compiled by rust
if DEBUG
    RUST_CPP_CLIENT = $(top_srcdir)/rust/client-proxy/target/debug/build/client-proxy-57a899ab70809e0a/out/rust_cpp/cpp_closures.o
else # for release 
    RUST_CPP_CLIENT = $(top_srcdir)/rust/client-proxy/target/release/build/client-proxy-e1025e79fd73a001/out/rust_cpp/cpp_closures.o
endif

violas_client_SOURCES = violas_client.cpp 
violas_client_CXXFLAGS = -std=c++17 -I$(top_srcdir)/cpp/src -I$(top_srcdir)/rust/client-proxy/src/ffi
violas_client_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib -L../../src/.libs $(CLIENT_PROXY_PATH)
violas_client_LDADD = $(CLIENT_PROXY) $(RUST_CPP_CLIENT) -lviolas_sdk