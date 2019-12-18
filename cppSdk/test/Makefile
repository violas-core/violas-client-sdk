CXX = clang++

test : clean
	$(CXX) -g -std=c++17 -stdlib=libc++ -Wall -Werror main.cpp ../src/libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../src -lstdc++fs
	 	
gcc : clean
	g++ -std=c++11 -g -Wall main.cpp ../src/libra_client.cpp -o test -L../../rust-client-proxy/target/debug -lclient_proxy -lboost_system -lboost_filesystem -I../src

mingw : clean
	x86_64-w64-mingw32-g++ -std=c++17 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

python : 
	g++ -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON \
	-o violas.so \
	../src/libra_client.cpp \
	-I/usr/include/python3.7m -lboost_python3-py37 \
	-L../../rust-client-proxy/target/debug/ \
	-I../src \
	-lclient_proxy -lstdc++fs

transfer : clean_transfer
	$(CXX) -g -std=c++17 -stdlib=libc++ -Wall -Werror \
	transfer.cpp ../libra_client.cpp \
	-o transfer \
	-I../ \
	-L../../rust-client-proxy/target/debug/ -lclient_proxy 
	#-lstdc++fs

clean :
	rm -f test log.txt

clean_transfer :
	rm -f transfer
