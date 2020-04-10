lib_LTLIBRARIES = libviolas_sdk.la libjniSdk.la liblibra_sdk.la

libviolas_sdk_la_SOURCES = violas_sdk.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
libviolas_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17
libviolas_sdk_la_LDFLAGS = -L$(top_srcdir)/rust-client-proxy/target/debug
libviolas_sdk_la_LIBADD = -lstdc++fs -lclient_proxy

liblibra_sdk_la_SOURCES = violas_sdk.cpp
#libcppSdk_la_HEADERS = violas_sdk.hpp
liblibra_sdk_la_CXXFLAGS = -Wall -Werror -std=c++17 -DLIB_NAME=Libra
liblibra_sdk_la_LDFLAGS = -L$(top_srcdir)/rust-client-proxy/target/debug
liblibra_sdk_la_LIBADD = -lstdc++fs -lclient_proxy

libjniSdk_la_SOURCES = violas_sdk.cpp jni_violas_sdk.cpp
libjniSdk_la_CXXFLAGS = -std=c++17 -fPIC \
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include \
                        -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
libjniSdk_la_LDFLAGS = -L$(top_srcdir)/rust-client-proxy/target/debug
libjniSdk_la_LIBADD = -lstdc++fs -lclient_proxy
