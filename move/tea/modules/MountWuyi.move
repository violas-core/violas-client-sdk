address 0x2 {

module MountWuyi {
    //use 0x1::DiemTimestamp;
    use 0x2::NonFungibleToken;
    
    const SECONDS_IN_ONE_DAY : u64 = 86400; // 24 * 60 * 60
    
    struct Tea has store, copy, drop, key {                
        kind : u8,              // 0 ,1, 2, 3, 4, 5
        manufacturer: vector<u8>,
        PA : vector<u8>,		// Production Area
        PD : u64,			    // Production Date
	    SN : vector<u8>,		// Sequence Number
    }
    
    fun verify_kind(kind: u8) {
        assert(kind < 5, 1000);
    }

    fun verify_manufacturer(_manufacturer: &vector<u8>) {

    }

    public fun mint_tea_nft(sig: &signer, 
                            kind: u8, 
                            manufacturer: vector<u8>, 
                            pa: vector<u8>,         // Production Area
                            pd: u64,                // Production Date
                            sn: vector<u8>,		    // Sequence Number
                            receiver: address) {
        
        verify_kind(kind);
        verify_manufacturer(&manufacturer);

        let tea = Tea {                        
            kind,
            manufacturer,
            PA : pa,
            PD : pd, // : DiemTimestamp::now_seconds() / SECONDS_IN_ONE_DAY * SECONDS_IN_ONE_DAY    // Keep date and remove time
            SN : sn
        } ;

        NonFungibleToken::mint<Tea>(sig, receiver, tea);
    }
}

}
