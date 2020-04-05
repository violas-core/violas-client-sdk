bin_PROGRAMS    = testnet test_jni

testnet_SOURCES = test_violas.cpp
testnet_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
testnet_LDFLAGS = -Wl,-rpath=./bin,-rpath=../bin,-rpath=/usr/local/lib \
                  -L../../src/.libs -L$(top_srcdir)/rust-client-proxy/target/debug
testnet_LDADD = -lcppSdk -l:libstdc++fs.a -lclient_proxy


test_jni_SOURCES = test_violas.cpp
test_jni_CXXFLAGS= -std=c++17 -I$(top_srcdir)/cppSdk/src
test_jni_LDFLAGS = -Wl,-rpath=./bin,-rpath=../bin,-rpath=/usr/local/lib \
                  -L../../src/.libs -L$(top_srcdir)/rust-client-proxy/target/debug \
                  -L../../../lib
test_jni_LDADD = -lviolas_sdk_jni -l:libstdc++fs.a -lclient_proxy