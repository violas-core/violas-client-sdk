mkdir -p build/debug;
mkdir -p build/release;
mkdir -p build/android-release;
cd build/debug; ../../configure CXX=c++ CXXFLAGS="-g -DDEBUG" --prefix=$PWD --enable-debug
cd ../release; ../../configure CXX=c++ CXXFLAGS="-O3" --prefix=$PWD
cd ../android-release; ../../configure CXXFLAGS="-O2 -DANDROID" --prefix=$PWD --host=aarch64-linux-android
