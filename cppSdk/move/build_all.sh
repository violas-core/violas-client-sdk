echo "build modules/coin_usd.move"
./build.sh currencies/vls_usd.move 0x0
mv move_build_output/modules/0_VLSUSD.mv currencies/vls_usd.mv

echo "build modules/vls.move"
./build.sh currencies/vls.move 0x0
mv move_build_output/modules/0_VLS.mv currencies/vls.mv

# echo "build modules/coin_eur.move"
# ./build.sh modules/coin_eur.move 0x0
# mv move_build_output/modules/0_VLSEUR.mv compiled/coin_eur.mv

echo "build currencies/register_currency.move"
./build.sh currencies/register_currency.move 0x0
mv move_build_output/scripts/main.mv currencies/register_currency.mv


# echo ""
# echo "Build for module Exchange ..."
# echo ""

echo "build exchange/exchange.move"
./build.sh exchange/exchange.move 0x0 exchange/exdep.move
mv move_build_output/modules/0_Exchange.mv exchange/exchange.mv

echo "build exchange/exdep.move"
./build.sh exchange/exdep.move 0x0
mv move_build_output/modules/0_ExDep.mv exchange/exdep.mv

echo "build exchange/initialize.move"
./build.sh exchange/initialize.move 0x0 exchange/exchange.move exchange/exdep.move
mv move_build_output/scripts/main.mv exchange/initialize.mv

echo "build exchange/add_currency.move"
./build.sh exchange/add_currency.move 0x0 exchange/exchange.move exchange/exdep.move
mv move_build_output/scripts/main.mv exchange/add_currency.mv

echo "build exchange/add_liquidity.move"
./build.sh exchange/add_liquidity.move 0x0 exchange/exchange.move exchange/exdep.move
mv move_build_output/scripts/main.mv exchange/add_liquidity.mv

echo "build exchange/remove_liquidity.move"
./build.sh exchange/remove_liquidity.move 0x0 exchange/exchange.move exchange/exdep.move
mv move_build_output/scripts/main.mv exchange/remove_liquidity.mv

echo "build exchange/swap.move"
./build.sh exchange/swap.move 0x0 exchange/exchange.move exchange/exdep.move
mv move_build_output/scripts/main.mv exchange/swap.mv

# echo "build scripts/exchange_token_to_violas_swap.move"
# ./build.sh scripts/exchange_token_to_violas_swap.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_token_to_violas_swap.mv

# echo "build scripts/exchange_violas_to_token_swap.move"
# ./build.sh scripts/exchange_violas_to_token_swap.move 0x0 modules/exchange.move
# mv move_build_output/scripts/main.mv compiled/exchange_violas_to_token_swap.mv


#echo "build bank.move"
#./build.sh exchange/bank.move 0x0
# mv  move_build_output/modules/0_ViolasBank.mv compiled/ViolasBank.mv

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