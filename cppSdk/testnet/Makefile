CXX = clang++

transfer : clean
	$(CXX) -O2 -std=c++17 -stdlib=libc++ -Wall -Werror \
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

gcc : clean
	g++ -O2 -std=c++17 -Wall \
	-o transfer \
	-I../src \
	-L../../rust-client-proxy/target/debug/ \
	src/transfer.cpp ../src/violas_sdk.cpp \
	-lclient_proxy -lstdc++fs\

clean :
	rm -f transfer log.txt