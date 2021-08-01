script {
    use 0x2::NonFungibleToken;
    
    fun nft_burn<T: store+copy+drop+key>(sig: signer, token_id:vector<u8>) {
        NonFungibleToken::burn<T>(&sig, &token_id);
    }

}