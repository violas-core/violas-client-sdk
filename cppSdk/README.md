# How to use cppSdk
1. include "libra_client.hpp" to your code
2. compiling 
    1. add libra_client.cpp to your project
    2. link rust-client-proxy/bin libclient_proxy.so    
    
for example, to compile cppSdk/test 
```
cd cppSdk/test
g++ -std=c++11 -g main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/bin -lclient_proxy -I..
```

# VStake for test
```
VStake's name is ABCUSD, address is b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e
VStake's name is HIJUDS, address is 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782
VStake's name is XYZUSD, address is f013ea4acf944fa6edafe01fae10713d13928ca5dff9e809dbcce8b12c2c45f1
VStake's name is BCDCAN, address is ad8e9520399689822b55bc783f03951c00fa2ae9eb997d477a2ff0bdc702a568
VStake's name is CDESDG, address is 15d3e4bea615b78c3782553df712a4f86d85280f11939e0b35756422575fc622
VStake's name is DEFHKD, address is e90e4f077bef23b32a6694a18a1fa34244532400869e4e8c87ce66d0b6c004bd
```