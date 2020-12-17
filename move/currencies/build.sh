echo "build modules/coin_usd.move"
../build.sh vls_usd.move 0x0
mv build/modules/0_VLSUSD.mv vls_usd.mv

echo "build usd.move"
../build.sh usd.move 0x0
mv build/modules/0_USD.mv usd.mv

echo "build usdt.move"
../build.sh usdt.move 0x0
mv build/modules/0_USDT.mv usdt.mv

echo "build currencies/register_currency.move"
../build.sh register_currency.move 0x0
mv build/scripts/main.mv register_currency.mv

rm -rf build