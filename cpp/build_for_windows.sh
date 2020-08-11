
../../configure --host=x86_64-w64-mingw32 CXXFLAGS="-O2 -static-libstdc++ -static-libgcc" LIBS="-lws2_32 -lcrypt32 -luserenv -lsecur32 -lncrypt" --prefix=$PWD