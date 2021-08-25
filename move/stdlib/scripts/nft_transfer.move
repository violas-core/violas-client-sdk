script {
    use 0x2::NonFungibleToken;
    
    fun nft_transfer_by_token_id<T: drop + key + store>(sig: signer, receiver:address, token_id:vector<u8>, metadata: vector<u8>) {
        NonFungibleToken::transfer<T>(&sig, receiver, &token_id, metadata);
    }

}

script {
    use 0x2::NonFungibleToken;

    fun nft_transfer_by_token_index<T: drop + store>(sig: signer, receiver: address, index: u64, metadata: vector<u8>) {
        NonFungibleToken::transfer_via_index<T>(&sig, receiver, index, metadata);
    }
}