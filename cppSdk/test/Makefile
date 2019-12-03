CXX = clang++

test : clean
	$(CXX) -g -std=c++17 -Wall -Werror main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../ -lstdc++fs
	# -stdlib=libc++ 
	
gcc : clean
	g++ -std=c++11 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -lboost_system -lboost_filesystem -I../

mingw : clean
	x86_64-w64-mingw32-g++ -std=c++11 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

clean :
	rm -f test log.txt