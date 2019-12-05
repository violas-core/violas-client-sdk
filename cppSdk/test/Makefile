CXX = clang++

test : clean
	$(CXX) -g -std=c++17 -Wall -Werror main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../ -lstdc++fs
	# -stdlib=libc++ 
	
gcc : clean
	g++ -std=c++11 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -lboost_system -lboost_filesystem -I../

mingw : clean
	x86_64-w64-mingw32-g++ -std=c++17 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

python : 
	$(CXX) -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON ../libra_client.cpp -o violas.so -I/usr/include/python3.6m -lboost_python3-py36

clean :
	rm -f test log.txt