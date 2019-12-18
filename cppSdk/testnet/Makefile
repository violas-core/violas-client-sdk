CXX = clang++

transfer : clean
	$(CXX) -O2 -std=c++17 -stdlib=libc++ -Wall -Werror \
	-o transfer \
	-I../src/ \
	-L../../rust-client-proxy/target/debug/ \
	-lclient_proxy \
	src/transfer.cpp ../src/libra_client.cpp

debug : clean
	clang++ -g -std=c++17 -Wall -Werror \
	-o transfer \
	-I../src/ \
	-L../../rust-client-proxy/target/debug/ \
	-lclient_proxy \
	src/transfer.cpp ../src/libra_client.cpp

gcc : clean
	g++ -O2 -std=c++17 -Wall \
	-o transfer \
	-I../src \
	-L../../rust-client-proxy/target/debug/ \
	src/transfer.cpp ../src/libra_client.cpp \
	-lclient_proxy \

clean :
	rm -f transfer