lib_LTLIBRARIES = libviolas_sdk_jni.la

libviolas_sdk_jni_la_SOURCES = io_violas_sdk_Client.cpp  io_violas_sdk_Token.cpp
libviolas_sdk_jni_la_CXXFLAGS = -std=c++17 \
                                -I$(top_srcdir)/cppSdk/src \
                                -I/usr/lib/jvm/java-11-openjdk-amd64/include \
                                -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
libviolas_sdk_jni_la_LDFLAGS = -L../../../cppSdk/src/.libs -L$(top_srcdir)/rust-client-proxy/target/debug
libviolas_sdk_jni_la_LIBADD = -l:libjniSdk.a -lclient_proxy -lstdc++fs

