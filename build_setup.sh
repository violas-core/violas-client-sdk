mkdir -p build/debug;
mkdir -p build/release;
cd build/debug; ../../configure CXX=clang++ CXXFLAGS="-g -DDEBUG" --prefix=$PWD --enable-debug
cd ../release; ../../configure CXX=clang++ CXXFLAGS="-O2" --prefix=$PWD