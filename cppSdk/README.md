# How to use cppSdk
1. include "libra_client.hpp" to your code
2. compiling 
    1. add libra_client.cpp to your project
    2. link rust-client-proxy/bin libclient_proxy.so    
    
3. for example, to compile cppSdk/test 
```
cd cppSdk/test
g++ -std=c++11 -g main.cpp ../libra_client.cpp -o test -L../../rust-client-proxy/bin -lclient_proxy -I..
```

# Token for test
```
Token's name is ABCUSD, address is b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e
Token's name is HIJUDS, address is 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782
Token's name is XYZUSD, address is f013ea4acf944fa6edafe01fae10713d13928ca5dff9e809dbcce8b12c2c45f1
Token's name is BCDCAN, address is ad8e9520399689822b55bc783f03951c00fa2ae9eb997d477a2ff0bdc702a568
Token's name is CDESDG, address is 15d3e4bea615b78c3782553df712a4f86d85280f11939e0b35756422575fc622
Token's name is DEFHKD, address is e90e4f077bef23b32a6694a18a1fa34244532400869e4e8c87ce66d0b6c004bd
```

## mnemonic for test
```
card tobacco message ordinary deer cement brick task memory prison bomb diet gallery letter office swing south provide grit decorate pistol grain apart enact;0
```

## Json format for Transaction

```
SignedTransaction { 
 raw_txn: RawTransaction { 
	sender: 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782, 
	sequence_number: 117, 
	payload: {, 
		transaction: <unknown transaction>, 
		args: [ 
			{ADDRESS: b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e},
			{U64: 1000}, 
		]
	}, 
	max_gas_amount: 140000, 
	gas_unit_price: 0, 
	expiration_time: 1575534689s, 
}, 
 public_key: Ed25519PublicKey(
    PublicKey(CompressedEdwardsY: [103, 152, 245, 13, 227, 228, 203, 163, 12, 92, 34, 28, 226, 155, 100, 235, 18, 188, 68, 190, 210, 51, 225, 20, 188, 157, 151, 29, 152, 138, 232, 234]), EdwardsPoint{
    	X: FieldElement51([35481458751993, 607875201348524, 1966670940540510, 824014234584630, 1132489848401057]),
    	Y: FieldElement51([1096088773040231, 1200135388370041, 302298247828079, 1932986280438111, 1880751842908537]),
    	Z: FieldElement51([1, 0, 0, 0, 0]),
    	T: FieldElement51([1133436349763556, 1866395507181278, 1789161918389494, 905625625542122, 1465107133842615])
    }),
), 
 signature: Ed25519Signature(
    Signature( R: CompressedEdwardsY: [177, 41, 100, 247, 217, 180, 161, 17, 72, 27, 1, 43, 117, 82, 132, 33, 184, 65, 74, 141, 111, 171, 49, 62, 236, 168, 84, 16, 246, 210, 70, 23], s: Scalar{
    	bytes: [24, 226, 39, 26, 141, 45, 198, 82, 191, 30, 169, 228, 208, 20, 111, 252, 14, 12, 26, 27, 198, 115, 60, 221, 142, 213, 222, 133, 9, 35, 113, 9],
    } ),
), 
 }

```

## Json format for Events
```
ContractEvent { key: EventKey([220, 140, 203, 162, 15, 80, 32, 231, 100, 186, 197, 36, 151, 99, 158, 91, 88, 12, 116, 46, 133, 101, 176, 100, 50, 253, 197, 115, 152, 181, 126, 38]), index: 35, type: Struct(StructTag { address: 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782, module: Identifier("DToken"), name: Identifier("AllinoneEvent"), type_params: [] }), event_data: "010000000000000075bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782e803000000000000000000000000000000000000" }
ContractEvent { key: EventKey([13, 139, 76, 150, 189, 177, 87, 230, 131, 172, 12, 175, 128, 86, 221, 190, 254, 159, 102, 97, 18, 196, 84, 81, 151, 253, 8, 103, 67, 115, 153, 62]), index: 52, type: Struct(StructTag { address: 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782, module: Identifier("DToken"), name: Identifier("AllinoneEvent"), type_params: [] }), event_data: "010000000000000075bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782e803000000000000000000000000000000000000" }
```