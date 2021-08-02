script {
    use 0x2::NonFungibleToken;
    
    fun nft_burn<T: store+drop>(sig: signer, token_id :vector<u8>) {
        
        NonFungibleToken::burn<T>(&sig, &token_id);
    }
}