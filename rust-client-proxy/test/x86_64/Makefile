CXX = clang++

test : clean
	$(CXX) -std=c++2a -stdlib=libc++ -g main.cpp libra_client.cpp -o test -L../../target/debug -lclient_proxy

clean :
	rm -f test