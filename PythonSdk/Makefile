violas : 
	clang++ -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON \
	-o violas.so \
	../cppSdk/src/violas_sdk.cpp \
	-I/usr/include/python3.7m -lboost_python3-py37 \
	-L../rust-client-proxy/target/debug/ \
	-I../cppSdk/src \
	-lclient_proxy -lstdc++fs

python36 : 
	clang++ -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON \
	-o violas.so \
	../cppSdk/src/violas_sdk.cpp \
	-I/usr/include/python3.6m -lboost_python3-py36 \
	-L../rust-client-proxy/target/debug/ \
	-I../cppSdk/src \
	-lclient_proxy -lstdc++fs

release : 
	clang++ -O3 -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON \
	-o violas.so \
	../cppSdk/src/violas_sdk.cpp \
	-I/usr/include/python3.7m -lboost_python3-py37 \
	-L../rust-client-proxy/target/release/ \
	-I../cppSdk/src \
	-l:libclient_proxy.a -lstdc++fs

	strip violas.so

clean :
	rm -f violas.so
	rm -rf __pycache__