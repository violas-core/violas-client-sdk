address 0x2 {

module Portrait {
    //use 0x1::DiemTimestamp;
    use 0x2::NonFungibleToken;
    
    const SECONDS_IN_ONE_DAY : u64 = 86400; // 24 * 60 * 60
    
    struct Portrait has store, copy, drop, key {                
        kind : u8,              // 0 ,1, 2, 3, 4, 5
        manufacturer: vector<u8>,       
        ipfs_cid: vector<u8>,		// refer to an address of a web page
    }
    
    fun verify_kind(kind: u8) {
        assert(kind < 5, 1000);
    }

    fun verify_manufacturer(_manufacturer: &vector<u8>) {

    }

    public fun mint_portrait_nft(sig: &signer, 
                            kind: u8, 
                            manufacturer: vector<u8>,                             
                            ipfs_cid: vector<u8>,	   
                            receiver: address) {
        
        verify_kind(kind);
        verify_manufacturer(&manufacturer);

        let Portrait = Portrait {                        
            kind,
            manufacturer,            
            ipfs_cid
        } ;

        NonFungibleToken::mint<Portrait>(sig, receiver, Portrait);
    }
}

}
