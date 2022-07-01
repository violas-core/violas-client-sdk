address 0x2 {

module Portrait2 {
    //use 0x1::DiemTimestamp;
    use 0x2::NonFungibleToken2;
    use DiemFramework::CoreAddresses;
    use Std::Event::{Self, EventHandle};
    use Std::Signer;

    const NFT_PUBLISHER: address = @0xA550C18;
    const SECONDS_IN_ONE_DAY : u64 = 86400; // 24 * 60 * 60
    
    struct Portrait has store, drop {
        description: vector<u8>,       
        ipfs_cid: vector<u8>,		// refer to an address of a web page
    }
    
    struct MintedEvent has store, drop { 
        token_id : vector<u8>,
        description: vector<u8>,
        ipfs_cid: vector<u8>,
        miner: address, 
        receiver: address,
    }

    struct Configuration has key {
        minted_events: EventHandle<MintedEvent>
    }

    public fun initialize(root: &signer) {
        CoreAddresses::assert_diem_root(root);
        
        move_to<Configuration>(root, Configuration { minted_events : Event::new_event_handle<MintedEvent>(root)});
    }

    public fun mint_portrait_nft(sig: &signer,                             
                                description: vector<u8>,                             
                                ipfs_cid: vector<u8>,	   
                                receiver: address)
    acquires Configuration {        

        let portrait = Portrait {
            description: copy description,            
            ipfs_cid: copy ipfs_cid
        } ;

        let token_id = NonFungibleToken2::mint<Portrait>(sig, receiver, portrait);

        emit_minted_event(token_id, description, ipfs_cid,  Signer::address_of(sig), receiver);
    }

    fun emit_minted_event(
        token_id: vector<u8>, 
        description: vector<u8>, 
        ipfs_cid: vector<u8>, 
        miner: address, 
        receiver: address) 
    acquires Configuration {
            
            let configuration = borrow_global_mut<Configuration>(NFT_PUBLISHER);

            Event::emit_event<MintedEvent>(&mut configuration.minted_events, 
                MintedEvent{ token_id, description, ipfs_cid, miner, receiver});
    }
}

}
