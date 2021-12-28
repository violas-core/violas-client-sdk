# ViolasClientSdk
Violas Client SDK for Android, IOS and PC.

SDK support the follwing programming language 
1. Java for Android
2. Swift for IOS, coming soon ...
3. C++ for Linux, MacOS and Windows.

## dependencies
```
sudo apt install cmake gcc-10 libstdc++-dev clang-12
```

## Build Rust code

1. git clone Violas source code 

2. git clone violas-client-sk in the same folder

3. cd rust/violas-client, run "cargo build --release"

4. in path ViolasClientSdk

## Build Cpp code
```
mkdir build && cd build 
cmake ../cpp/
make
```

# Compiling Violas Client on Windows 

```
1. 	Install Vistual Studio 2019 Community with C++ and clang
2. 	Install Rust
3. 	Install ActivePerl 5.28.1
4. 	Install Go 1.13.4
5. 	Download https://github.com/protocolbuffers/protobuf/releases/download/v3.11.1/protoc-3.11.1-win64.zip
   5.1 unzip protoc-3.11.1-win64.zip
   5.2 copy "protoc-3.11.1-win64/bin/protoc.exe" to "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.24.28314\bin\Hostx64\x64"
   5.3 copy folder "protoc-3.11.1-win64\include\google" to "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.24.28314\include"
6. 	Run "cargo build"
	grpcio with version 0.4.7 can be compiled successfully, but grpcio with version 0.5.0-alpha.5 cannot be compiled successfully.

```

## How is this project organized?
```
├── android
│   ├── app
│   ├── gradle
│   └── sdk
├── build
│   ├── debug
│   └── release
├── cpp
│   ├── command
│   ├── framework
│   ├── lib
│   ├── nft
│   ├── nft-store
│   ├── sdk
│   ├── test
│   ├── testnet
│   └── vls
├── doc
├── java
│   ├── src
│   └── test
├── move
│   ├── bank
│   ├── currencies
│   ├── exchange
│   ├── oracle
│   ├── stdlib
│   ├── swap
│   └── tea
├── python
└── rust
    ├── violas-client
    └── violas-oracle
```
