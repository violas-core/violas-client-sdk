lib_LTLIBRARIES = libviolas_sdk.la libjniSdk.la liblibra_sdk.la

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust-client-proxy/target/debug #-L$(top_srcdir)/rust-client-proxy/target/x86_64-pc-windows-gnu/release
    CLIENT_PROXY =
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust-client-proxy/target/release
    CLIENT_PROXY = 
endif

libviolas_sdk_la_SOURCES = violas_sdk.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
libviolas_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 
libviolas_sdk_la_LDFLAGS = $(CLIENT_PROXY_PATH)
libviolas_sdk_la_LIBADD = $(CLIENT_PROXY) 

liblibra_sdk_la_SOURCES = violas_sdk.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
liblibra_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -DLIB_NAME=Libra
liblibra_sdk_la_LDFLAGS = $(CLIENT_PROXY_PATH)
liblibra_sdk_la_LIBADD = $(CLIENT_PROXY)

libjniSdk_la_SOURCES = violas_sdk.cpp jni_violas_sdk.cpp
libjniSdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -fPIC\
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include \
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
libjniSdk_la_LDFLAGS = $(CLIENT_PROXY_PATH)
libjniSdk_la_LIBADD =  $(CLIENT_PROXY)


