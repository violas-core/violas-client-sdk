script {
    use 0x2::NonFungibleToken;
    
    fun nft_register<T: store>(sig: signer, amount: u64, admin: address, symbol: vector<u8>) {
        NonFungibleToken::register<T>(&sig, true, amount, admin, symbol);
    }

}