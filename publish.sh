echo "install all binaries to" $1

mkdir $1 $1/bin $1/lib/ $1/move -p
echo "created all dirs."

cp rust/violas-client/target/release/libviolas_client.so $1/lib/
echo "copied libclient_proxy.so ."$1"/lib"

cp rust/violas-oracle/target/release/violas-oracle $1/bin
echo "copied violas-oracle to "$1"/bin"

cp build/release/lib $1 -r
echo "copied build/release/lib to "$1"/lib"

cp build/release/bin $1 -r
echo "copied build/release/bin to "$1"/bin"

cp ../Violas/target/release/cli $1/bin
cp ../Violas/target/release/diem-node $1/bin
cp ../Violas/target/release/move-build $1/bin
cp ../Violas/target/release/diem-swarm $1/bin
echo "copied ../Violas/target/release/bin to "$1"/bin"

strip $1/lib/*.so $1/bin/* 

#copy all files in folder move and then remove *.move and *.sh in target folder 
cp -r move/ $1 && find $1/move -name "*.move" | xargs rm && find $1/move -name "*.sh" | xargs rm
echo "copied all compiled move contract files"

cp -r python $1
echo "copied python example."

cp -r cpp/testnet/mnemonic $1
echo "copy all mnemonic files"
 