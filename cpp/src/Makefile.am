lib_LTLIBRARIES = libviolas_sdk.la liblibra_sdk.la libpy_violas_sdk.la #libjniSdk.la

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/debug #-L$(top_srcdir)/rust/client-proxy/target/x86_64-pc-windows-gnu/release
    RUST_CPP_CLIENT =  $(top_srcdir)/rust/violas-client/target/debug/build/violas-client-33336450db207f62/out/rust_cpp/cpp_closures.o
else
    # CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release
    # RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/aarch64-linux-android/release/build/violas-client-1ea0705af08078ac/out/rust_cpp/cpp_closures.o
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/violas-client/target/release
    RUST_CPP_CLIENT = $(top_srcdir)/rust/violas-client/target/release/build/violas-client-609eda45df9365c2/out/rust_cpp/cpp_closures.o
endif



libviolas_sdk_la_SOURCES = violas_sdk2.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
libviolas_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -I$(top_srcdir)/rust/violas-client/src/ffi
libviolas_sdk_la_LDFLAGS = $(CLIENT_PROXY_PATH)
libviolas_sdk_la_LIBADD = $(RUST_CPP_CLIENT)

liblibra_sdk_la_SOURCES = violas_sdk2.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
liblibra_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -DLIB_NAME=Libra -I$(top_srcdir)/rust/violas-client/src/ffi
liblibra_sdk_la_LDFLAGS = $(CLIENT_PROXY_PATH) 
liblibra_sdk_la_LIBADD = $(RUST_CPP_CLIENT)

# libjniSdk_la_SOURCES = violas_sdk.cpp jni_violas_sdk.cpp
# libjniSdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -fPIC\
#                         -I/usr/lib/jvm/java-11-openjdk-amd64/include \
#                         -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/ \
#                         -I$(top_srcdir)/rust/client-proxy/src/ffi
# libjniSdk_la_LDFLAGS = $(CLIENT_PROXY_PATH)
# libjniSdk_la_LIBADD =  $(CLIENT_PROXY)

libpy_violas_sdk_la_SOURCES = py_violas_sdk.cpp
libpy_violas_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -fPIC\
                        -I$(top_srcdir)/rust/violas-client/src/ffi \
                        -I/usr/include/python3.8                        
libpy_violas_sdk_la_LDFLAGS = $(CLIENT_PROXY_PATH) -Wl,-rpath=./lib,-rpath=../lib
libpy_violas_sdk_la_LIBADD =  $(RUST_CPP_CLIENT) -lboost_python38 #-lboost_python3-py36
