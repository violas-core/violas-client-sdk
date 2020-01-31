all : libcppSdk

libcppSdk : violas_sdk.o
	ar -crv lib/libcppSdk.a violas_sdk.o

violas_sdk.o :
	$(CXX) -O2 -std=c++17 \
	-c src/violas_sdk.cpp \
	-o violas_sdk.o

android : libcppSdk-arm64v8a.a

libcppSdk-arm64v8a.a : violas_sdk.o
	$(AR) -crv lib/libcppSdk-arm64v8a.a violas_sdk.o

clean :
	rm -f lib/libcppSdk.a lib/libcppSdk-arm64v8a.a violas_sdk.o