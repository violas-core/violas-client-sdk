address 0x2 {

module MountWuyi {
    use 0x1::DiemTimestamp;
    use 0x2::NonFungibleToken;
    
    const SECONDS_IN_ONE_DAY : u64 = 86400; // 24 * 60 * 60
    
    struct Tea has store, copy, drop {        
        identity : vector<u8>,
        kind : u8,  // 0 ,1, 2, 3, 4, 5
        manufacturer: vector<u8>,
        date : u64,
    }
    
    fun verify_kind(kind: u8) {
        assert(kind < 5, 1000);
    }

    fun verify_manufacturer(_manufacturer: &vector<u8>) {

    }

    public fun mint_tea_nft(sig: &signer, identity: vector<u8>, kind: u8, manufacturer: vector<u8>, receiver: address) {
        
        verify_kind(kind);
        verify_manufacturer(&manufacturer);

        let tea = Tea {            
            identity,
            kind,
            manufacturer,
            date : DiemTimestamp::now_seconds() / SECONDS_IN_ONE_DAY * SECONDS_IN_ONE_DAY    // Keep date and remove time
        } ;

        NonFungibleToken::mint<Tea>(sig, receiver, tea);
    }
}

}
