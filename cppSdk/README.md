# How to use Violas sdk
1. include header files and use namespace
```cpp
#include "violas_sdk.h"

using namespace Violas;
```
2. compiling and linking 
```    
-lviolas_sdk -LViolasClientSdk/build/release/lib    
```    
# How to use Libra sdk
1. include header files and use namespace
```cpp
#define LIB_NAME Libra
#include "violas_sdk.h"

using namespace Libra;
```
2. compiling and linking 
```    
-llibra_sdk -LViolasClientSdk/build/release/lib    
``` 

# How to use both Violas SDK and Libra SDK
1. keep in mind that Violas SDK and Libra SDK must be called in different cpp file of a project, see example files test_libra.cpp and test_violas.cpp in path testnet/src, check Makefile.am and learn how to build these files together and generate binary testnet  
2. compiling and link
```
-lviolas_sdk -llibra_sdk -LViolasClientSdk/build/release/lib
```

## Json format for Transaction

```json
TransactionView {
    version: 164726,
    transaction: UserTransaction {
        sender: "d1427ed9b2c35605f3d0e76a898bb7f2",
        signature_scheme: "Scheme::Ed25519",
        signature: "fbb1b097d612d94d5eab6d920648978849fb68383966a96004c8208322c19975e637f87ef418639d1e0d77f359446bde9a42af5eac625fa009f0f59780343a0d",
        public_key: "ca3f706428a300f39c495b6df896383ee9e40aaae8750c1766bde57c6162013b",
        sequence_number: 19,
        max_gas_amount: 400000,
        gas_unit_price: 0,
        expiration_time: 1586507104,
        script_hash: "ce38cc1b29eb2003ddc8d2d05b5fc7d8c0c97a48231ab9ab699a42658554cfc6",
        script: PeerToPeer {
            receiver: "41bb3520bb6d1185c9a8a82ef2003c33",
            auth_key_prefix: BytesView(
                "",
            ),
            amount: 1000000,
            metadata: BytesView(
                "",
            ),
        },
    },
    events: [
        EventView {
            key: BytesView(
                "0100000000000000d1427ed9b2c35605f3d0e76a898bb7f2",
            ),
            sequence_number: 3,
            transaction_version: 164726,
            data: SentPayment {
                amount: 1000000,
                receiver: BytesView(
                    "41bb3520bb6d1185c9a8a82ef2003c33",
                ),
                metadata: BytesView(
                    "",
                ),
            },
        },
        EventView {
            key: BytesView(
                "000000000000000041bb3520bb6d1185c9a8a82ef2003c33",
            ),
            sequence_number: 11,
            transaction_version: 164726,
            data: ReceivedPayment {
                amount: 1000000,
                sender: BytesView(
                    "d1427ed9b2c35605f3d0e76a898bb7f2",
                ),
                metadata: BytesView(
                    "",
                ),
            },
        },
    ],
    vm_status: EXECUTED,
    gas_used: 0,
}
```
