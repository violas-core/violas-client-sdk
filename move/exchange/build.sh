rm *.mv

echo "build exchange.move"
../build.sh exchange.move
mv build/modules/0_Exchange.mv exchange.mv

# echo "build "
# ../build.sh  0x0
# mv build/modules/0_ExDep.mv exdep.mv

echo "build initialize.move"
../build.sh initialize.move exchange.move 
mv build/scripts/main.mv initialize.mv

echo "build add_currency.move"
../build.sh add_currency.move exchange.move 
mv build/scripts/main.mv add_currency.mv

echo "build add_liquidity.move"
../build.sh add_liquidity.move exchange.move 
mv build/scripts/main.mv add_liquidity.mv

echo "build remove_liquidity.move"
../build.sh remove_liquidity.move exchange.move 
mv build/scripts/main.mv remove_liquidity.mv

echo "build change_rewarder.move"
../build.sh change_rewarder.move exchange.move 
mv build/scripts/main.mv change_rewarder.mv

echo "build set_next_rewardpool.move"
../build.sh set_next_rewardpool.move exchange.move 
mv build/scripts/main.mv set_next_rewardpool.mv

echo "build set_pool_alloc_point.move"
../build.sh set_pool_alloc_point.move exchange.move 
mv build/scripts/main.mv set_pool_alloc_point.mv

echo "build swap.move"
../build.sh swap.move exchange.move 
mv build/scripts/main.mv swap.mv

echo "build withdraw_mine_reward.move"
../build.sh withdraw_mine_reward.move exchange.move 
mv build/scripts/main.mv withdraw_mine_reward.mv

rm -rf build