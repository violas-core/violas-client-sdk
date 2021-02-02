echo "build modules/coin_usd.move"
../build.sh vls_usd.move 0x0
mv build/modules/0_VLSUSD.mv vls_usd.mv

echo "build usd.move"
../build.sh usd.move 0x0
mv build/modules/0_USD.mv usd.mv

echo "build usdt.move"
../build.sh usdt.move 0x0
mv build/modules/0_USDT.mv usdt.mv

echo "build register_currency.move"
../build.sh register_currency.move 0x0
mv build/scripts/main.mv register_currency.mv

echo "build vls_initialize_timestamp.move"
../build.sh vls_initialize_timestamp.move 0x0
mv build/scripts/main.mv vls_initialize_timestamp.mv

echo "build mine_vls.move"
../build.sh mine_vls.move 0x0
mv build/scripts/main.mv mine_vls.mv

rm -rf build