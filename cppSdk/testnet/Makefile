

transfer : clean
	clang++ -g -std=c++17 -stdlib=libc++ -Wall -Werror \
	-o transfer \
	-I../src/ \
	-L../../rust-client-proxy/target/debug/ \
	-lclient_proxy \
	src/transfer.cpp ../src/violas_sdk.cpp

debug : clean
	clang++ -g -std=c++17 -Wall -Werror \
	-o transfer \
	-I../src/ \
	-L../../rust-client-proxy/target/debug/ \
	src/transfer.cpp ../src/violas_sdk.cpp \
	-lclient_proxy -lstdc++fs\

release : clean
	clang++ -O3 -std=c++17 -stdlib=libc++ -Wall -Werror \
	-o transfer \
	-I../src/ \
	src/transfer.cpp ../src/violas_sdk.cpp \
	-L../../rust-client-proxy/target/release/ \
	-l:libclient_proxy.a -lpthread -ldl
	
	strip transfer

android : clean
	$(CXX) -O3 -std=c++17 -stdlib=libc++ -Wall -Werror \
	-o transfer \
	-I../src/ \
	src/transfer.cpp ../src/violas_sdk.cpp \
	-L../../rust-client-proxy/target/aarch64-linux-android/release/ \
	-L./ \
	-l:libc++fs.a -l:libclient_proxy.a -llog 
	
	strip arm_transfer

gcc : clean
	g++ -O2 -std=c++17 -Wall \
	-o transfer \
	-I../src \
	-L../../rust-client-proxy/target/debug/ \
	src/transfer.cpp ../src/violas_sdk.cpp \
	-lclient_proxy -lstdc++fs\

clean :
	rm -f transfer log.txt