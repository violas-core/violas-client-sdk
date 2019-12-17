CXX = clang++

transfer : clean
	$(CXX) -g -std=c++17 -stdlib=libc++ -Wall -Werror \
	-o transfer \
	-I../ \
	-L../../rust-client-proxy/target/debug/ \
	-lclient_proxy \
	transfer.cpp ../libra_client.cpp
	
clean :
	rm -f transfer