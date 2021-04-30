rm *.mv

echo "build usd.move"
../build.sh usd.move 0x0
mv build/modules/0_USD.mv usd.mv

echo "build usdt.move"
../build.sh usdt.move 0x0
mv build/modules/0_USDT.mv usdt.mv

echo "build usdeu.move"
../build.sh usdeu.move 0x0
mv build/modules/0_USDEU.mv usdeu.mv

echo "build usdeur.move"
../build.sh usdeur.move 0x0
mv build/modules/0_USDEUR.mv usdeur.mv

echo "build register_currency.move"
../build.sh register_currency.move 0x0
mv build/scripts/main.mv register_currency.mv

rm -rf build