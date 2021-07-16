echo "build all bank contracts"

echo "remove all mv files"
rm *.mv

echo "build bank.move"
../build.sh bank.move ../oracle/oracle.move
mv  build/modules/0_ViolasBank.mv bank.mv

echo "build borrow.move"
../build.sh borrow.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv borrow.mv

echo "build create_token.move"
../build.sh create_token.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv create_token.mv

echo "build enter_bank.move"
../build.sh enter_bank.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv enter_bank.mv

echo "build exit_bank.move"
../build.sh exit_bank.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv exit_bank.mv

echo "build liquidate_borrow.move"
../build.sh liquidate_borrow.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv liquidate_borrow.mv

echo "build lock.move"
../build.sh lock.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv lock.mv

echo "build mint.move"
../build.sh mint.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv mint.mv

echo "build publish.move"
../build.sh publish.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv publish.mv

echo "build register_libra_token.move"
../build.sh register_libra_token.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv register_libra_token.mv

echo "build redeem.move"
../build.sh redeem.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv redeem.mv

echo "build repay_borrow.move"
../build.sh repay_borrow.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv repay_borrow.mv

echo "build update_collateral_factor.move"
../build.sh update_collateral_factor.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv update_collateral_factor.mv

echo "build update_price.move"
../build.sh update_price.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv update_price.mv

echo "build update_price_from_oracle.move"
../build.sh update_price_from_oracle.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv update_price_from_oracle.mv

echo "building set_incentive_rate.move"
../build.sh set_incentive_rate.move bank.move ../oracle/oracle.move
mv build/scripts/main.mv set_incentive_rate.mv

rm -rf build