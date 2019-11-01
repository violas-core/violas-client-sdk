CXX = clang++

test : clean
	$(CXX) -std=c++11 -stdlib=libc++ -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

clean :
	rm -f test log.txt