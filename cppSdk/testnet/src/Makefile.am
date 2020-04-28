bin_PROGRAMS    = testnet test_jni transfer #test_rust_cpp

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust-client-proxy/target/debug
    CLIENT_PROXY = -lclient_proxy
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust-client-proxy/target/release
    CLIENT_PROXY = -lclient_proxy
endif

testnet_SOURCES = test_violas.cpp test_libra.cpp main.cpp
testnet_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
testnet_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L$(top_builddir)/cppSdk/src/.libs $(CLIENT_PROXY_PATH)
testnet_LDADD = -lviolas_sdk -llibra_sdk -l:libstdc++fs.a $(CLIENT_PROXY)

test_jni_SOURCES = test_violas.cpp test_libra.cpp main.cpp
test_jni_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
test_jni_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib \
                  -L../../src/.libs -L../../../lib $(CLIENT_PROXY_PATH)                  
test_jni_LDADD = -lviolas_sdk_jni -llibra_sdk -l:libstdc++fs.a $(CLIENT_PROXY)

transfer_SOURCES = transfer.cpp
transfer_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
transfer_LDFLAGS = -Wl,-rpath=./bin,-rpath=../bin,-rpath=/usr/local/lib \
                  -L../../src/.libs $(CLIENT_PROXY_PATH) 
transfer_LDADD = -lviolas_sdk -l:libstdc++fs.a $(CLIENT_PROXY)

# test_rust_cpp_SOURCES = test_rust_cpp.cpp
# test_rust_cpp_CXXFLAGS = -std=c++17 -I$(top_srcdir)/rust-client-proxy/src/ffi
# test_rust_cpp_LDFLAGS = -Wl,-rpath=./lib,-rpath=../lib,-rpath=/usr/local/lib $(CLIENT_PROXY_PATH)
# test_rust_cpp_LDADD = -llibra_sdk $(CLIENT_PROXY)