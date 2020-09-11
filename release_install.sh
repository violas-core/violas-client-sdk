echo "install all binaries to" $1

mkdir $1 $1/bin $1/lib/ $1/move -p
echo "created all dirs."

cp rust/client-proxy/target/release/libclient_proxy.so $1/lib/
echo "copied libclient_proxy.so ."$1"/lib"

cp rust/oracle-service/target/release/violas-oracle $1/bin
echo "copied violas-oracle to "$1"/bin"

cp build/release/lib $1 -r
echo "copied build/release/lib to "$1"/lib"

cp build/release/bin $1 -r
echo "copied build/release/bin to "$1"/bin"

strip $1/lib/*.so $1/bin/* 

cp -r move/ $1
echo "copied all compiled move contract files"

cp -r cpp/testnet/mnemonic $1
echo "copy all mnemonic files"
 