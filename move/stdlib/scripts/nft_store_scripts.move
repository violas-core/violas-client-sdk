script {    
    use 0x2::NftStore;    
    
    fun nft_store_initialize(sig: signer) { 
        NftStore::initialize(&sig);
    }
}

script {    
    use 0x2::NftStore;    
    
    fun nft_store_register_nft<NFT: store>(sig: signer) { 
        NftStore::register<NFT>(&sig);
    }
}


script {    
    use 0x2::NftStore;    
    
    fun nft_store_accept<NFT: store>(sig: signer) { 
        NftStore::accept<NFT>(&sig);
    }
}

script {    
    use 0x2::NftStore;
    use Std::FixedPoint32;

    fun nft_store_make_order<NFT: key+store, Token: store>( 
                                sig: signer, 
                                nft_token_id: vector<u8>,
                                price : u64,
                                sale_incentive_numerator: u64,
                                sale_incentive_denominator: u64,
                                ) {
        
        let sale_incentive = FixedPoint32::create_from_rational(
                            sale_incentive_numerator, 
                            sale_incentive_denominator);

        NftStore::make_order<NFT, Token>(&sig, 
                            price, 
                            sale_incentive,
                            &nft_token_id);
    }


}

script {    
    use 0x2::NftStore;    
    
    fun nft_store_revoke_order<NFT: key+store>(sig: signer, order_id: vector<u8>) { 
        NftStore::revoke_order<NFT>(&sig, order_id);
    }
}