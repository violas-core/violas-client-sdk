address 0x2 {

module Meta42 {
    use Std::BCS;
    use Std::Compare;
    use Std::Event::{Self, EventHandle};
    use Std::Hash;
    use Std::Option::{Self, Option};
    use Std::Signer;
    use Std::Vector;

    struct MintedTokenEvent has drop, store {

    }

    struct BurnedTokenEvent has drop, store {

    }

    struct SharedTokenEvent has drop, store {
        sender: address,
        receiver: address,
        token_id : vector<u8>,
        metadata: vector<u8>
    }
    
    struct GlobalInfo has key {
        shared_events : EventHandle<SharedTokenEvent>
    }

    struct Token has copy, store {
        hdfs : vector<u8>
    }

    struct AccountInfo has key {
        tokens : vector<Token>
    }

    fun get_admind_address() : address {
        @0xaa
    }

    fun compute_token_id(token: &Token) : vector<u8> {
        let bytes = BCS::to_bytes(token);
        Hash::sha3_256(bytes)
    }

    public fun initialize(sig : &signer) {
        let sender = Signer::address_of(sig);

        assert(sender == get_admind_address(), 10000);

        if(!exists<GlobalInfo>(sender))
            move_to<GlobalInfo>(sig, GlobalInfo { 
                shared_events : Event::new_event_handle<SharedTokenEvent>(sig)
            });        
    }

    public fun accept_account_info(sig: &signer) {
        if(!exists<AccountInfo>(Signer::address_of(sig))) {
            move_to<AccountInfo>(sig, AccountInfo {
                tokens : Vector::empty<Token>()
            });
        }
    }
    /*
     * fun : mint_token
     * description : mint a token into current account
     *
     */
    public fun mint_token(sig: &signer, hdfs: vector<u8>)
    acquires AccountInfo {
        let sender = Signer::address_of(sig);

        accept_account_info(sig);

        let account_info = borrow_global_mut<AccountInfo>(sender);

        Vector::push_back<Token>(&mut account_info.tokens, Token { hdfs });
    }   

    fun get_token_index_by_id(owner: address, token_id: vector<u8>) : Option<u64> 
    acquires AccountInfo {
        let account_info = borrow_global_mut<AccountInfo>(owner);

        let length = Vector::length<Token>(&account_info.tokens);
        let i = 0;

        while (i < length) {
            
            let token = Vector::borrow<Token>(&account_info.tokens, i);
            
            let hash  = compute_token_id(token);

            if( Compare::cmp_bcs_bytes(&hash, &token_id) == 0)
                return Option::some(i);
            
            i = i + 1;
        };

        Option::none()
    }

    fun emit_shared_token_evnet(sender: address, receiver: address, token_id: vector<u8>, metadata: vector<u8>) 
    acquires GlobalInfo {
        
        assert(exists<GlobalInfo>(get_admind_address()), 10005);
        
        let global_info = borrow_global_mut<GlobalInfo>(get_admind_address());

        Event::emit_event<SharedTokenEvent>(&mut global_info.shared_events, SharedTokenEvent { sender, receiver, token_id, metadata});
    }    

    public fun share_token_by_id(sig: &signer, receiver: address, token_id: vector<u8>, metadata: vector<u8>)
    acquires AccountInfo, GlobalInfo {
        let sender = Signer::address_of(sig);

        let opt_index = get_token_index_by_id(sender, token_id);

        assert(Option::is_some(&opt_index), 10000);

        let index = Option::extract<u64>(&mut opt_index);

        share_token_by_index(sig, receiver, index, metadata)
    }

    public fun share_token_by_index(sig: &signer, receiver: address, index: u64, metadata: vector<u8>)
    acquires AccountInfo, GlobalInfo {
        let sender = Signer::address_of(sig);        

        assert(exists<AccountInfo>(sender), 10002);

        let sender_info = borrow_global_mut<AccountInfo>(sender);

        // copy a token from sender
        let token = *Vector::borrow(&sender_info.tokens, index);
        let token_id = compute_token_id(&token);

        // get the account info from receiver
        let receiver_info = borrow_global_mut<AccountInfo>(receiver);

        Vector::push_back<Token>(&mut receiver_info.tokens, token);
        
        emit_shared_token_evnet(sender, receiver, token_id, metadata)
    }
}

}

