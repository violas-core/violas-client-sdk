CXX = clang++

test : clean
	$(CXX) -g -std=c++17 -stdlib=libc++ -Wall -Werror main.cpp ../src/violas_sdk.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../src -lstdc++fs
	 	
gcc : clean
	g++ -std=c++11 -g -Wall main.cpp ../src/violas_sdk.cpp -o test -L../../rust-client-proxy/target/debug -lclient_proxy -lboost_system -lboost_filesystem -I../src

mingw : clean
	x86_64-w64-mingw32-g++ -std=c++17 -g -Wall main.cpp ../violas_sdk.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

clean :
	rm -f test log.txt
