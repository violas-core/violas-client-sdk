script {    
    use 0x2::MountWuyi;

    fun mint_mountwuyi_tea_nft(sig: signer,                 
                kind: u8, 
                manufacturer: vector<u8>,
                pa: vector<u8>,         // Production Area
                pd: u64,                // Production Date
                sn: vector<u8>,		    // Sequence Number
                url: vector<u8>,	    // url
                receiver: address) {        
        MountWuyi::mint_tea_nft(&sig, kind, manufacturer, pa, pd, sn, url, receiver);
    }

}