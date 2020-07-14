echo "build all bank contracts"

echo "remove all mv files"
rm *.mv

echo "build bank.move"
../build.sh bank.move 0x0
mv  move_build_output/modules/0_ViolasBank.mv bank.mv

echo "build borrow.move"
../build.sh borrow.move 0x0 bank.move
mv move_build_output/scripts/main.mv borrow.mv

echo "build create_token.move"
../build.sh create_token.move 0x0 bank.move
mv move_build_output/scripts/main.mv create_token.mv

echo "build enter_bank.move"
../build.sh enter_bank.move 0x0 bank.move
mv move_build_output/scripts/main.mv enter_bank.mv

echo "build exit_bank.move"
../build.sh exit_bank.move 0x0 bank.move
mv move_build_output/scripts/main.mv exit_bank.mv

echo "build liquidate_borrow.move"
../build.sh liquidate_borrow.move 0x0 bank.move
mv move_build_output/scripts/main.mv publish.mv

echo "build lock.move"
../build.sh lock.move 0x0 bank.move
mv move_build_output/scripts/main.mv lock.mv

echo "build mint.move"
../build.sh mint.move 0x0 bank.move
mv move_build_output/scripts/main.mv mint.mv

echo "build publish.move"
../build.sh publish.move 0x0 bank.move
mv move_build_output/scripts/main.mv publish.mv

echo "build register_libra_token.move"
../build.sh register_libra_token.move 0x0 bank.move
mv move_build_output/scripts/main.mv register_libra_token.mv

echo "build repay_borrow.move"
../build.sh repay_borrow.move 0x0 bank.move
mv move_build_output/scripts/main.mv repay_borrow.mv

echo "build update_collateral_factor.move"
../build.sh update_collateral_factor.move 0x0 bank.move
mv move_build_output/scripts/main.mv update_collateral_factor.mv

echo "build update_price.move"
../build.sh update_price.move 0x0 bank.move
mv move_build_output/scripts/main.mv update_price.mv

rm -rf move_build_output