clear
# add NDK path to $PATH
export PATH=$PATH:~/Android/ndk-toolchain/bin
# build wit release
cargo build --release --target aarch64-linux-android
# strip so file
aarch64-linux-android-strip target/aarch64-linux-android/release/libclient_proxy.so
# link to android project
ln -b  target/aarch64-linux-android/release/libclient_proxy.so ../AndroidClientSdk/libraclient/src/main/jniLibs/arm64-v8a/libclient_proxy.so