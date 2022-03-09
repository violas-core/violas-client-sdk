script {    
    use 0x2::Portrait;

    fun mint_portrait_nft(sig: signer,                
                description: vector<u8>,
                ipfs_cid: vector<u8>,
                receiver: address) {

        Portrait::mint_portrait_nft(&sig, description, ipfs_cid, receiver);
    }

}