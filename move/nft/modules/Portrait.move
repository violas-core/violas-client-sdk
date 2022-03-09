address 0x2 {

module Portrait {
    //use 0x1::DiemTimestamp;
    use 0x2::NonFungibleToken;
    
    const SECONDS_IN_ONE_DAY : u64 = 86400; // 24 * 60 * 60
    
    struct Portrait has store, drop {
        description: vector<u8>,       
        ipfs_cid: vector<u8>,		// refer to an address of a web page
    }
    
    public fun mint_portrait_nft(sig: &signer,                             
                                description: vector<u8>,                             
                                ipfs_cid: vector<u8>,	   
                                receiver: address) {        

        let portrait = Portrait {
            description,            
            ipfs_cid
        } ;

        NonFungibleToken::mint<Portrait>(sig, receiver, portrait);
    }
}

}
