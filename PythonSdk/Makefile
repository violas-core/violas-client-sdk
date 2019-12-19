python : 
	clang++ -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON \
	-o violas_sdk.so \
	../cppSdk/src/violas_sdk.cpp \
	-I/usr/include/python3.7m -lboost_python3-py37 \
	-L../rust-client-proxy/target/debug/ \
	-I../cppSdk/src \
	-lclient_proxy -lstdc++fs

clean :
	rm -f violas_sdk.so