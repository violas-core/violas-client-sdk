script {
    use 0x1::Exchange;

    fun deposit_liquidity<Token1, Token2>(
        sender : &signer,
        token1_amount : u64,
        token2_amount : u64 ) {
        Exchange::deposit_liquidity<Token1, Token2>(sender, token1_amount, token2_amount);
    }
}