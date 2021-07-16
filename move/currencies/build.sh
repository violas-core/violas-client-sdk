rm *.mv

echo "build usd.move"
../build.sh usd.move
mv build/modules/0_USD.mv usd.mv

echo "build usdt.move"
../build.sh usdt.move
mv build/modules/0_USDT.mv usdt.mv

echo "build usdeu.move"
../build.sh usdeu.move
mv build/modules/0_USDEU.mv usdeu.mv

echo "build usdeur.move"
../build.sh usdeur.move
mv build/modules/0_USDEUR.mv usdeur.mv

echo "build register_currency.move"
../build.sh register_currency.move
mv build/scripts/main.mv register_currency.mv

rm -rf build