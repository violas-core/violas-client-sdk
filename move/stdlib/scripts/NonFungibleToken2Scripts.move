script {
    use 0x2::NonFungibleToken2;
    
    fun nft2_initialize(sig: signer) {
        
        NonFungibleToken2::initialize(&sig);
    }
}

script {
    use 0x2::NonFungibleToken2;
    
    fun nft2_register<T: store>(sig: signer, amount: u64, admin: address, symbol: vector<u8>) {
        NonFungibleToken2::register<T>(&sig, true, amount, admin, symbol);
    }

}

script {
    use 0x2::NonFungibleToken2;
    
    fun nft2_accept<T: store>(sig: signer) {
        
        NonFungibleToken2::accept<T>(&sig);
    }
}

script {
    use 0x2::NonFungibleToken2;
    
    fun nft2_burn<T: store+drop>(sig: signer, token_id :vector<u8>) {
        
        NonFungibleToken2::burn<T>(&sig, &token_id);
    }
}

script {
    use 0x2::NonFungibleToken2;
    
    fun nft2_transfer_by_token_id<T: drop + store>(sig: signer, receiver:address, token_id:vector<u8>, metadata: vector<u8>) {
        NonFungibleToken2::transfer<T>(&sig, receiver, &token_id, &metadata);
    }

}

script {
    use 0x2::NonFungibleToken2;

    fun nft2_transfer_by_token_index<T: drop + store>(sig: signer, receiver: address, index: u64, metadata: vector<u8>) {
        NonFungibleToken2::transfer_by_index<T>(&sig, receiver, index, &metadata);
    }
}