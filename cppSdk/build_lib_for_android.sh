export PATH=~/Android/ndk-toolchain/bin:$PATH

export CXX=aarch64-linux-android26-clang++
export AR=aarch64-linux-android-ar
export CXXFLAGS=-DANDROID

make clean
make android 
