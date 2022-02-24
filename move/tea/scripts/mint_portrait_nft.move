script {    
    use 0x2::Portrait;

    fun mint_mountwuyi_tea_nft(sig: signer,                 
                kind: u8, 
                manufacturer: vector<u8>,               
                url: vector<u8>,	    // url
                receiver: address) {        
        Portrait::mint_tea_nft(&sig, kind, manufacturer, pa, pd, sn, url, receiver);
    }

}