script {    
    use 0x2::MountWuyi;

    fun mint_mountwuyi_tea_nft(sig: signer, 
                identity: vector<u8>,
                kind: u8, 
                manufacturer: vector<u8>, 
                receiver: address) {        
        MountWuyi::mint_tea_nft(&sig, identity, kind, manufacturer, receiver);
    }

}