# How to udpate rust code from libra client

1. git merge-file src/client_proxy.rs ../../libra/testsuite/cli/src/client_proxy.rs src/client_proxy.rs
   
   git merge-file src/libra_client.rs ../../libra/testsuite/cli/src/libra_client.rs src/libra_client.rs


2. resolve the conflicts

# Build for Windows on Ubuntu
1. Dependencies

   sudo apt install mingw-w64

   rustup target install x86_64-pc-windows-gnu

2. build

   cargo build --release --target=x86_64-pc-windows-gnu

   ln -s target/x86_64-pc-windows-gnu/release target/lib