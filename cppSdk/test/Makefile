CXX = clang++

test : clean
	$(CXX) -g -std=c++17 -Wall -Werror main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../ -lstdc++fs
	# -stdlib=libc++ 
	
gcc : clean
	g++ -std=c++11 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -lboost_system -lboost_filesystem -I../

mingw : clean
	x86_64-w64-mingw32-g++ -std=c++17 -g -Wall main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../

python : 
	g++ -g -std=c++17 -fPIC -shared -Wall -Werror -DPYTHON -o violas.so \
	../libra_client.cpp \
	-I/usr/include/python3.6m -lboost_python3-py36 \
	-L../../rust-client-proxy/target/debug/ -lclient_proxy \
	-I../ -lstdc++fs

transfer : clean_transfer
	$(CXX) -g -std=c++17 -Wall -Werror transfer.cpp ../libra_client.cpp -o transfer -L../../rust-client-proxy/target/debug/ -lclient_proxy -I../ -lstdc++fs

clean :
	rm -f test log.txt

clean_transfer :
	rm -f transfer
