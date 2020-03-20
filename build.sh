mkdir build;mkdir build/debug; mkdir build/release;
cd build/debug; ../../configure CXX=clang++ CXXFLAGS="-g" --prefix=$PWD
cd ../release; ../../configure CXX=clang++ CXXFLAGS="-O2" --prefix=$PWD --enable-silent-rules