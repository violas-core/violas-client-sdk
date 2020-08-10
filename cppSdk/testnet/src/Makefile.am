bin_PROGRAMS    = testnet test_jni transfer violas_client

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/debug
    CLIENT_PROXY = -lclient_proxy 
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/release
    CLIENT_PROXY =  -lclient_proxy
endif

testnet_SOURCES = test_violas.cpp test_libra.cpp test_exchange.cpp test_bank.cpp main.cpp
testnet_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
testnet_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L$(top_builddir)/cppSdk/src/.libs $(CLIENT_PROXY_PATH) 
testnet_LDADD = -lviolas_sdk -llibra_sdk $(CLIENT_PROXY) -lstdc++fs

test_jni_SOURCES = test_violas.cpp test_libra.cpp test_exchange.cpp test_bank.cpp main.cpp
test_jni_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
test_jni_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L../../src/.libs -L../../../lib $(CLIENT_PROXY_PATH)                  
test_jni_LDADD = -lviolas_sdk_jni -lviolas_sdk -llibra_sdk $(CLIENT_PROXY) -lstdc++fs

transfer_SOURCES = transfer.cpp
transfer_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
transfer_LDFLAGS = -Wl,-rpath=./bin,-rpath=../bin,-rpath=/usr/local/lib \
                  -L../../src/.libs $(CLIENT_PROXY_PATH) 
transfer_LDADD = -lviolas_sdk $(CLIENT_PROXY) -lstdc++fs

# file cpp_closures.o compiled by rust 
RUST_CPP_CLIENT = $(top_srcdir)/rust/client-proxy/target/debug/build/client-proxy-2cbbb9b5fffd816c/out/rust_cpp/cpp_closures.o
# for release $(top_srcdir)/rust/client-proxy/target/release/build/client-proxy-f60ce46fb23ec7a6/out/rust_cpp/cpp_closures.o

violas_client_SOURCES = violas_client.cpp 
violas_client_CXXFLAGS = -std=c++17 -I$(top_srcdir)/rust/client-proxy/src/ffi
violas_client_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib $(CLIENT_PROXY_PATH)
violas_client_LDADD = $(CLIENT_PROXY) $(RUST_CPP_CLIENT)