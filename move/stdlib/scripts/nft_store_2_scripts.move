script {    
    use 0x2::NftStore2;    
    
    fun nft_store_2_initialize( sig: signer) {

        NftStore2::initialize(&sig);
        
    }
}

script {    
    use 0x2::NftStore2;    
    use Std::FixedPoint32;

    fun nft_store_2_register_nft<NFT: store>(sig: signer, 
                                            fee_rate_numerator:u64, 
                                            fee_rate_denominator :u64) { 
            
            NftStore2::register<NFT>(&sig, 
            
            FixedPoint32::create_from_rational(fee_rate_numerator, fee_rate_denominator));
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
                            &nft_token_id,
                            price);
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
    
    fun nft_store_2_trade_order<NFT: store, Token>(
        sender_sig : signer,                                                
        order_id: vector<u8>) { 
        
        NftStore2::trade_order<NFT, Token>(&sender_sig, &order_id);
    }
}