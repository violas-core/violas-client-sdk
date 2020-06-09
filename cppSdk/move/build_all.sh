echo "build modules/coin_usd.move"
./build.sh modules/coin_usd.move 0x0
mv move_build_output/modules/0_USD.mv compiled/coin_usd.mv

echo "build modules/coin_eur.move"
./build.sh modules/coin_eur.move 0x0
mv move_build_output/modules/0_EUR.mv compiled/coin_eur.mv

echo "build modules/coin_gbp.move"
./build.sh modules/coin_gbp.move 0x0 
mv move_build_output/modules/0_GBP.mv compiled/coin_gbp.mv

echo "build scripts/register_currency.move"
./build.sh scripts/register_currency.move 0x0
mv move_build_output/scripts/main.mv compiled/register_currency.mv


# echo ""
# echo "Build for module Exchange ..."
# echo ""

# echo "build modules/exchange.move"
# ./build.sh modules/exchange.move 0x0
# mv move_build_output/modules/0_Exchange.mv compiled/exchange.mv

# echo "build srcripts/exchange_initialize.move"
# ./build.sh scripts/exchange_initialize.move 0x0 modules/exchange.move 
# mv move_build_output/scripts/main.mv compiled/exchange_initialize.mv

# echo "build scripts/exchange_publish_reserve.move"
# ./build.sh scripts/exchange_publish_reserve.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_publish_reserve.mv

# echo "build scripts/exchange_add_liquidity.move"
# ./build.sh scripts/exchange_add_liquidity.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_add_liquidity.mv

# echo "build scripts/exchange_remove_liquidity.move"
# ./build.sh scripts/exchange_remove_liquidity.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_remove_liquidity.mv

# echo "build scripts/exchange_token_to_token_swap.move"
# ./build.sh scripts/exchange_token_to_token_swap.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_token_to_token_swap.mv

# echo "build scripts/exchange_token_to_violas_swap.move"
# ./build.sh scripts/exchange_token_to_violas_swap.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_token_to_violas_swap.mv

# echo "build scripts/exchange_violas_to_token_swap.move"
# ./build.sh scripts/exchange_violas_to_token_swap.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_violas_to_token_swap.mv


echo "build bank.move"
./build.sh modules/bank.move 0x0
mv  move_build_output/modules/0_ViolasBank.mv compiled/ViolasBank.mv

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

# echo "build modules/violas.move"
# ./build.sh modules/violas.move 0x0
# mv move_build_output/modules/0_Violas.mv violas.mv

echo ""
echo "done"
rm -r move_build_output