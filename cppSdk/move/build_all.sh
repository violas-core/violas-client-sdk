# echo "build token.move"
# ./build.sh token.move 0x0
# mv output/transaction_0_module_ViolasToken.mv token.mv

# echo "build publish.move"
# ./build.sh publish.move 0x0 token.move
# mv output/transaction_0_script.mv publish.mv

# echo "build mint.move"
# ./build.sh mint.move 0x0 token.move
# mv output/transaction_0_script.mv mint.mv

# echo "build transfer.move"
# ./build.sh transfer.move 0x0 token.move
# mv output/transaction_0_script.mv transfer.mv

# echo "build create_token.move"
# ./build.sh create_token.move 0x0 token.move
# mv output/transaction_0_script.mv create_token.mv

echo "build modules/violas.move"
./build.sh modules/violas.move 0x0
mv move_build_output/modules/0_Violas.mv violas.mv

# echo "build violas_initialize.move"
# ./build.sh scripts/violas_initialize.move 0x0 modules/violas.move
# mv move_build_output/scripts/main.mv compiled/violas_initialize.mv

echo "build modules/coin_usd.move"
./build.sh modules/coin_usd.move 0x0
mv move_build_output/modules/0_USD.mv compiled/coin_usd.mv

echo "build modules/coin_eur.move"
./build.sh modules/coin_eur.move 0x0
mv move_build_output/modules/0_EUR.mv compiled/coin_eur.mv

echo "build modules/exchange.move"
./build.sh modules/exchange.move 0x0
mv move_build_output/modules/0_Exchange.mv compiled/exchange.mv

echo "build srcripts/exchange_initialize.move"
./build.sh scripts/exchange_initialize.move 0x0 modules/exchange.move 
mv move_build_output/scripts/main.mv compiled/exchange_initialize.mv

echo "done"
rm -r move_build_output