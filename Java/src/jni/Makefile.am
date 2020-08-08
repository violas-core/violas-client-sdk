lib_LTLIBRARIES = libviolas_sdk_jni.la

if DEBUG
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/debug
    CLIENT_PROXY = -lclient_proxy
else
    CLIENT_PROXY_PATH = -L$(top_srcdir)/rust/client-proxy/target/release
    CLIENT_PROXY = -lclient_proxy
endif


libviolas_sdk_jni_la_SOURCES = io_violas_sdk_Client.cpp  io_violas_sdk_Token.cpp
libviolas_sdk_jni_la_CXXFLAGS = -Wall -Werror -std=c++17 -fPIC\
                                -I$(top_srcdir)/cppSdk/src \
                                -I/usr/lib/jvm/java-11-openjdk-amd64/include \
                                -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
libviolas_sdk_jni_la_LDFLAGS = -L../../../cppSdk/src/.libs $(CLIENT_PROXY_PATH)
libviolas_sdk_jni_la_LIBADD = -l:libjniSdk.a $(CLIENT_PROXY)

