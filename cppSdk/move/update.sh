cp ../../../exchange-matchengine/violas_move/bank.move ./modules
#cp ../../../exchange-matchengine/violas_move/*.mv ./

cp ../../../violas-dex-core/contract/exchange.move modules/
cp ../../../violas-dex-core/contract/initialize.move scripts/exchange_initialize.move
cp ../../../violas-dex-core/contract/publish_reserve.move scripts/exchange_publish_reserve.move
cp ../../../violas-dex-core/contract/add_liquidity.move scripts/exchange_add_liquidity.move
cp ../../../violas-dex-core/contract/remove_liquidity.move scripts/exchange_remove_liquidity.move
cp ../../../violas-dex-core/contract/token_to_token_swap.move scripts/exchange_token_to_token_swap.move
cp ../../../violas-dex-core/contract/token_to_violas_swap.move scripts/exchange_token_to_violas_swap.move
cp ../../../violas-dex-core/contract/violas_to_token_swap.move scripts/exchange_violas_to_token_swap.move


