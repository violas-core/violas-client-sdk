echo "build modules/coin_usd.move"
../build.sh vls_usd.move 0x0
mv move_build_output/modules/0_VLSUSD.mv vls_usd.mv

echo "build usd.move"
../build.sh usd.move 0x0
mv move_build_output/modules/0_USD.mv usd.mv

echo "build usdt.move"
../build.sh usdt.move 0x0
mv move_build_output/modules/0_USDT.mv usdt.mv

echo "build libra.move"
../build.sh libra.move 0x0
mv move_build_output/modules/0_LIBRA.mv libra.mv

echo "build add_currency_for_designated_dealer.move"
../build.sh add_currency_for_designated_dealer.move 0x0
mv move_build_output/scripts/main.mv add_currency_for_designated_dealer.mv

echo "build currencies/register_currency.move"
../build.sh register_currency.move 0x0
mv move_build_output/scripts/main.mv register_currency.mv

rm -rf move_build_output