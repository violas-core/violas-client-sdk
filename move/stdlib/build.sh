rm modules/*.mv scripts/*.mv

echo "build allow_publishing_module.move"
../build.sh scripts/allow_publishing_module.move 0x0
mv build/scripts/main.mv scripts/allow_publishing_module.mv

echo "build allow_custom_script.move"
../build.sh scripts/allow_custom_script.move 0x0
mv build/scripts/main.mv scripts/allow_custom_script.mv

echo "build create_designated_dealer_ex.move"
../build.sh scripts/create_designated_dealer_ex.move 0x0
mv build/scripts/create_designated_dealer_ex.mv scripts/

# echo "build distribute_vls_from_community.move"
../build.sh scripts/distribute_vls_from_community.move 0x0 ../bank/bank.move ../oracle/oracle.move ../exchange/exchange.move
mv build/scripts/distribute_vls_from_community.mv scripts/

echo "build add_currency_for_designated_dealer.move"
../build.sh scripts/add_currency_for_designated_dealer.move 0x0
mv build/scripts/add_currency_for_designated_dealer.mv scripts/

echo "build vls_initialize_timestamp.move"
../build.sh scripts/vls_initialize_timestamp.move 0x0
mv build/scripts/main.mv scripts/vls_initialize_timestamp.mv

echo "build mine_vls.move"
../build.sh scripts/mine_vls.move 0x0
mv build/scripts/main.mv scripts/mine_vls.mv

echo "build recover_vls_fees_to_association.move"
../build.sh scripts/recover_vls_fees_to_association.move 0x0
mv build/scripts/recover_vls_fees_to_association.mv scripts/

echo "build scripts/register_nft.move"
../build.sh scripts/register_nft.move 0x0 modules/NonFungibleToken.move modules/Map.move
mv build/scripts/register_nft.mv scripts/

# echo "build SortedLinkedList.move"
# ../build.sh remove_liquidity.move 0x0 exchange.move exdep.move
# mv build/scripts/main.mv remove_liquidity.mv

echo "build modules/Compare.move"
../build.sh modules/Compare.move 0x0
mv build/modules/0_Compare.mv modules/Compare.mv

echo "build modules/Set.move"
../build.sh modules/Set.move 0x0
mv build/modules/0_Set.mv modules/Set.mv

echo "build modules/Map.move"
../build.sh modules/Map.move 0x0
mv build/modules/0_Map.mv modules/Map.mv

echo "build modules/NonFungibleToken.move"
../build.sh modules/NonFungibleToken.move 0x0  modules/SimpleSortedLinkedList.move modules/SortedLinkedList.move modules/Map.move
mv build/modules/0_NonFungibleToken.mv modules/NonFungibleToken.mv

rm -rf build