
echo "build modules/MountWuyi.move"
../build.sh modules/MountWuyi.move 0x0 ../stdlib/modules/NonFungibleToken.move ../stdlib/modules/Map.move
mv build/modules/0_MountWuyi.mv modules/MountWuyi.mv

echo "build scripts/mint_mountwuyi_tea_nft.move"
../build.sh scripts/mint_mountwuyi_tea_nft.move 0x0 modules/MountWuyi.move ../stdlib/modules/NonFungibleToken.move ../stdlib/modules/Map.move
mv build/scripts/mint_mountwuyi_tea_nft.mv scripts/

rm -rf build