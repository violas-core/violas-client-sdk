# ViolasClientSdk
Violas Client SDK for Android, IOS and PC.

SDK support the follwing programming language 
1. Java for Android
2. Swift for IOS
3. C++ for Linux, MacOS and Windows.



## Build

1. download Libra source code and change file libra/client/src/grpc_client.rs

#### original  

```Rust
pub(crate) fn get_account_blob
```

#### modified  

```rust
pub fn get_account_blob
```


2. open ViolasClientSdk/rust-client-proxy/cargo.toml, replace all reference for 'Libra' with correct path.

3. run "cargo build"

   ​


