echo "build token.move"
./build.sh token.move 0x0
mv output/transaction_0_module_ViolasToken.mv token.mv

echo "build publish.move"
./build.sh publish.move 0x0 token.move
mv output/transaction_0_script.mv publish.mv

echo "build mint.move"
./build.sh mint.move 0x0 token.move
mv output/transaction_0_script.mv mint.mv

echo "build transfer.move"
./build.sh transfer.move 0x0 token.move
mv output/transaction_0_script.mv transfer.mv

echo "build create_token.move"
./build.sh create_token.move 0x0 token.move
mv output/transaction_0_script.mv create_token.mv

