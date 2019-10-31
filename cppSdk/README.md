#How to use cppSdk
1. include "libra_client.hpp" to your code
2. compiling refer to compile test program
    cd cppSdk/test
    g++ -std=c++11 -g main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/bin -lclient_proxy -I..
