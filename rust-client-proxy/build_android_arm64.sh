# add NDK path to $PATH
export PATH=$PATH:~/Android/ndk-toolchain/bin
# build wit release
cargo build --release --target aarch64-linux-android