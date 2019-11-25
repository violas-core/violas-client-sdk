CXX = clang++

test : clean
	$(CXX) -std=c++17 -stdlib=libc++ --debug -Wall -Werror main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

gcc : clean
	g++ -std=c++17 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../ -lstdc++fs

clean :
	rm -f test log.txt