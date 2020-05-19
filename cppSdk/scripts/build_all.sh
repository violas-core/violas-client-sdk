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

echo "build violas.move"
./build.sh violas.move 0x0
mv move_build_output/modules/0_Violas.mv violas.mv

echo "build violas_initialize.move"
./build.sh violas_initialize.move 0x0 violas.move
mv move_build_output/scripts/main.mv violas_initialize.mv

echo "build coin_usd.move"
./build.sh coin_usd.move 0x0
mv move_build_output/modules/0_USD.mv coin_usd.mv

echo "build coin_eur.move"
./build.sh coin_eur.move 0x0
mv move_build_output/modules/0_EUR.mv coin_eur.mv