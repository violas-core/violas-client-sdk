echo "build oracle.move"
../build.sh oracle.move
mv build/modules/0_Oracle.mv oracle.mv

echo "build update_exchange_rate.move"
../build.sh update_exchange_rate.move oracle.move
mv build/scripts/main.mv update_exchange_rate.mv

echo "build test_exchange_rate.move"
../build.sh test_exchange_rate.move oracle.move
mv build/scripts/main.mv test_exchange_rate.mv

# echo "build currencies/register_currency.move"
# ../build.sh register_currency.move 0x0
# mv build/scripts/main.mv register_currency.mv

rm -rf build