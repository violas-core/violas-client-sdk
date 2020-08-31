echo "build exchange.move"
../build.sh exchange.move 0x0 exdep.move
mv move_build_output/modules/0_Exchange.mv exchange.mv

echo "build exdep.move"
../build.sh exdep.move 0x0
mv move_build_output/modules/0_ExDep.mv exdep.mv

echo "build initialize.move"
../build.sh initialize.move 0x0 exchange.move exdep.move
mv move_build_output/scripts/main.mv initialize.mv

echo "build add_currency.move"
../build.sh add_currency.move 0x0 exchange.move exdep.move
mv move_build_output/scripts/main.mv add_currency.mv

echo "build add_liquidity.move"
../build.sh add_liquidity.move 0x0 exchange.move exdep.move
mv move_build_output/scripts/main.mv add_liquidity.mv

echo "build remove_liquidity.move"
../build.sh remove_liquidity.move 0x0 exchange.move exdep.move
mv move_build_output/scripts/main.mv remove_liquidity.mv

echo "build swap.move"
../build.sh swap.move 0x0 exchange.move exdep.move
mv move_build_output/scripts/main.mv swap.mv

rm -rf move_build_output