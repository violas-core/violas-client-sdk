script {
    use 0x2::NonFungibleToken;
    
    fun register_nft<T: store>(sig: signer, amount: u64, admin: address) {
        NonFungibleToken::register<T>(&sig, true, amount, admin);
    }

}