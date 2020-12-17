rm *.mv

echo "build allow_publishing_module.move"
../build.sh allow_publishing_module.move 0x0
mv build/scripts/main.mv allow_publishing_module.mv

echo "build allow_custom_script.move"
../build.sh allow_custom_script.move 0x0
mv build/scripts/main.mv allow_custom_script.mv

echo "build create_designated_dealer_ex.move"
../build.sh create_designated_dealer_ex.move 0x0
mv build/scripts/create_designated_dealer_ex.mv .

# echo "build distribute_vls_from_community.move"
# ../build.sh distribute_vls_from_community.move 0x0 ../bank/bank.move ../oracle/oracle.move
# mv build/scripts/distribute_vls_from_community.mv .

echo "build add_currency_for_designated_dealer.move"
../build.sh add_currency_for_designated_dealer.move 0x0
mv build/scripts/add_currency_for_designated_dealer.mv .

echo "build recover_vls_fees_to_association.move"
../build.sh recover_vls_fees_to_association.move 0x0
mv build/scripts/recover_vls_fees_to_association.mv .

# echo "build remove_liquidity.move"
# ../build.sh remove_liquidity.move 0x0 exchange.move exdep.move
# mv build/scripts/main.mv remove_liquidity.mv

# echo "build swap.move"
# ../build.sh swap.move 0x0 exchange.move exdep.move
# mv build/scripts/main.mv swap.mv

rm -rf build