echo "biulding swap.move ..."
../build.sh exchange.move
mv build/modules/0_Exchange.mv exchange.mv

echo "biulding vlswap.move ..."
../build.sh module/vlswap.move Fee.move
mv build/modules/0_Exchange.mv exchange.mv

echo "biulding verify_sqrt.move ..."
../build.sh verify_sqrt.move exchange.move
mv build/scripts/verify_sqrt.mv .

echo "biulding initialize.move ..."
../build.sh initialize.move exchange.move
mv build/scripts/initialize.mv .

echo "biulding add_reserve.move ..."
../build.sh add_reserve.move exchange.move
mv build/scripts/add_reserve.mv .

echo "biulding deposit_liquidity.move ..."
../build.sh deposit_liquidity.move exchange.move
mv build/scripts/deposit_liquidity.mv .

echo "biulding withdraw_liquidity.move ..."
../build.sh withdraw_liquidity.move exchange.move
mv build/scripts/withdraw_liquidity.mv .

echo "biulding swap.move ..."
../build.sh swap.move exchange.move
mv build/scripts/swap.mv .
mv build/scripts/swap3.mv .
mv build/scripts/swap4.mv .

rm -rf build
echo "cleaned ./build"