#How to use cppSdk
1. include "libra_client.hpp" to your code
2. compiling 
    1. add libra_client.cpp to your project
    2. link rust-client-proxy/bin libclient_proxy.so    
    
    for example, to compile cppSdk/test 
```
    cd cppSdk/test
    g++ -std=c++11 -g main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/bin -lclient_proxy -I..
```

#6 VStake for test
    ABCUSD；HIJUDS；XYZUSD；BCDCAN；CDESDG；DEFHKD