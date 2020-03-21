lib_LTLIBRARIES = libcppSdk.la libjniSdk.la

libcppSdk_la_SOURCES = violas_sdk.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
libcppSdk_la_CXXFLAGS = -Wall -Werror -std=c++17
libcppSdk_la_LDFLAGS = -L$(top_srcdir)/rust-client-proxy/target/debug
libcppSdk_la_LIBADD = -lstdc++fs -l:libclient_proxy.a

libjniSdk_la_SOURCES = violas_sdk.cpp jni_violas_sdk.cpp
libjniSdk_la_CXXFLAGS = -std=c++17 -fPIC \
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include \
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
libjniSdk_la_LDFLAGS = -L$(top_srcdir)/rust-client-proxy/target/release
libjniSdk_la_LIBADD = -lstdc++fs -l:libclient_proxy.a
