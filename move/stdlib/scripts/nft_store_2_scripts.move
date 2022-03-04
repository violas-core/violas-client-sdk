script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_initialize(
        sig: signer,
        provider_parent_address: address) { 

        NftStore2::initialize(&sig, provider_parent_address);
    }
}

script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_register_nft<NFT: store>(sig: signer) { 
        NftStore2::register<NFT>(&sig);
    }
}


script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_accept<NFT: store>(sig: signer) { 
        NftStore2::accept<NFT>(&sig);
    }
}

script {    
    use 0x2::NftStore2;    

    fun nft_store_2_make_order<NFT: store, Token>( 
                                sig: signer, 
                                nft_token_id: vector<u8>,
                                price : u64                                
                                ) {

        NftStore2::make_order<NFT, Token>(&sig, 
                            price,                             
                            &nft_token_id);
    }

}

script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_revoke_order<NFT: store>(sig: signer, order_id: vector<u8>) { 
        NftStore2::revoke_order<NFT>(&sig, order_id);
    }
}

script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_trade_order<NFT: store, Token>(sender_sig : signer,                                                
                                                order_id: vector<u8>) { 
        
        NftStore2::trade_order<NFT, Token>(&sender_sig, &order_id);
    }
}