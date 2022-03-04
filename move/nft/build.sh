rm modules/*.mv scripts/*.m -f

echo "build modules/MountWuyi.move"
../build.sh modules/MountWuyi.move ../stdlib/modules/NonFungibleToken.move ../stdlib/modules/Map.move ../stdlib/modules/Compare.move
mv build/modules/0_MountWuyi.mv modules/MountWuyi.mv

echo "build modules/MountWuyi.move"
../build.sh modules/Portrait.move ../stdlib/modules/NonFungibleToken.move ../stdlib/modules/Map.move ../stdlib/modules/Compare.move
mv build/modules/0_Portrait.mv modules/Portrait.mv

echo "build scripts/mint_mountwuyi_tea_nft.move"
../build.sh scripts/mint_mountwuyi_tea_nft.move modules/MountWuyi.move ../stdlib/modules/NonFungibleToken.move ../stdlib/modules/Map.move ../stdlib/modules/Compare.move
mv build/scripts/mint_mountwuyi_tea_nft.mv scripts/

rm -rf build
# move compile ../stdlib/modules/ modules/ --mode diem
# rm build storage/ -rf