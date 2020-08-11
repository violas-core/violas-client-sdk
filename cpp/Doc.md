# Libra

## 1.client::create 
    创建libra客户端
```c++
create(const std::string &host,                     //远程验证节点IP地址
           uint16_t port,                           //端口
           const std::string &validator_set_file,   //验证节点集合文件
           const std::string &faucet_account_file,  //铸币账户的配置文件
           bool sync_on_wallet_recovery,            //是否同步recovery
           const std::string &faucet_server,        //libra节点的获取测试币的URL
           const std::string &mnemonic_file);       //本地助记文件

```

## 2. client::test_validator_connection
      测试网络连接  

## 3. client::create_next_account
    创建一个新的账户
## 4. client::transfer_coins_int

```
/// Transfer num_coins from sender account to receiver.
//  If is_blocking = true, it will keep querying validator till the sequence number is bumped up in validator.
    //  return : account's index and sequence number
    virtual std::pair<uint64_t, uint64_t>
    transfer_coins_int(uint64_t sender_account_ref_id, // the reference id of account
                       uint256 receiver_address,       // the address of receiver
                       uint64_t micro_coins,           // a millionth of a coin
                       uint64_t gas_unit_price = 0,    // set gas unit price or 0
                       uint64_t max_gas_amount = 0,    // set the max gas account or 0
                       bool is_blocking = true) = 0;   // true for sync, fasle for async

```



# Violas

## Token::create

```c++
static std::shared_ptr<Token> 
create(Libra::client_ptr client,	//Libra client object
       uint256 governor_addr,		//governor address
       const std::string &name);	//Token name
```

## Token::deploy, 

```
deploy(uint64_t account_index); //account index
```

## Token::publish

```
publish(uint64_t account_index); //account index
```

## Token::mint

```
mint(uint64_t account_index, // account index
	 uint256 address, 		// receiver address
	 uint64_t amount_micro_coin)		// amount, 1
```

## Token::transfer

```
transfer(uint64_t account_index, // account index
		uint256 address, 			//receiver address
		uint64_t amount_micro_coin)	//
```

## Token::get_account_balance

```
get_account_balance(uint64_t account_index) // account index
```



# Transaction JSON foramt

```json
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

# Events JSON format

```json
ContractEvent { key: EventKey([220, 140, 203, 162, 15, 80, 32, 231, 100, 186, 197, 36, 151, 99, 158, 91, 88, 12, 116, 46, 133, 101, 176, 100, 50, 253, 197, 115, 152, 181, 126, 38]), index: 35, type: Struct(StructTag { address: 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782, module: Identifier("DToken"), name: Identifier("AllinoneEvent"), type_params: [] }), event_data: "010000000000000075bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782e803000000000000000000000000000000000000" }
ContractEvent { key: EventKey([13, 139, 76, 150, 189, 177, 87, 230, 131, 172, 12, 175, 128, 86, 221, 190, 254, 159, 102, 97, 18, 196, 84, 81, 151, 253, 8, 103, 67, 115, 153, 62]), index: 52, type: Struct(StructTag { address: 75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782, module: Identifier("DToken"), name: Identifier("AllinoneEvent"), type_params: [] }), event_data: "010000000000000075bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782b9e3266ca9f28103ca7c9bb9e5eb6d0d8c1a9d774a11b384798a3c4784d5411e75bea7a9c432fe0d94f13c6d73543ea8758940e9b622b70dbbafec5ffbf74782e803000000000000000000000000000000000000" }
```

