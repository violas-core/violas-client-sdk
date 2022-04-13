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
        token_id :  vector<u8>,
        hdfs :      vector<u8>,
        minter:     address
    }

    struct BurnedTokenEvent has drop, store {

    }

    //
    //  Shared Token Event
    //  saved under global admin account
    struct SharedTokenEvent has drop, store {
        sender :    address,
        receiver :  address,
        token_id :  vector<u8>,
        metadata :  vector<u8>
    }
    //
    //  Sent token event
    //  saved under the account who sent  a sharing token
    //
    struct SentTokenEvent has drop, store {        
        receiver :  address,
        token_id :  vector<u8>,
        metadata :  vector<u8>
    }
    //
    //  Received token event
    //  saved under the account who received a sharing token
    //
    struct ReceivedTokenEvent has drop, store {
        sender :    address,        
        token_id :  vector<u8>,
        metadata :  vector<u8>
    }
    //
    //  Global Info
    //  saved under the admin account
    //
    struct GlobalInfo has key {
        minted_events : EventHandle<MintedTokenEvent>,
        shared_events : EventHandle<SharedTokenEvent>
    }

    struct Token has copy, store {
        hdfs : vector<u8>
    }

    struct AccountInfo has key {
        tokens : vector<Token>,
        minted_events : EventHandle<MintedTokenEvent>,
        sent_events : EventHandle<SentTokenEvent>,
        received_events : EventHandle<ReceivedTokenEvent>
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
                minted_events : Event::new_event_handle<MintedTokenEvent>(sig),
                shared_events : Event::new_event_handle<SharedTokenEvent>(sig)
            });        
    }

    public fun accept(sig: &signer) {
        if(!exists<AccountInfo>(Signer::address_of(sig))) {
            move_to<AccountInfo>(sig, AccountInfo {
                tokens : Vector::empty<Token>(),
                minted_events : Event::new_event_handle<MintedTokenEvent>(sig),
                sent_events : Event::new_event_handle<SentTokenEvent>(sig),
                received_events : Event::new_event_handle<ReceivedTokenEvent>(sig)
            });
        }
    }

    fun emit_minted_event(minter: address, token_id: vector<u8>, hdfs: vector<u8>)
    acquires GlobalInfo {
        
        assert(exists<GlobalInfo>(get_admind_address()), 10005);
        
        let global_info = borrow_global_mut<GlobalInfo>(get_admind_address());

        Event::emit_event<MintedTokenEvent>(&mut global_info.minted_events, MintedTokenEvent { token_id, hdfs, minter} );
    }

    /*
     * fun : mint_token
     * description : mint a token into current account
     *
     */
    public fun mint_token(sig: &signer, hdfs: vector<u8>)
    acquires AccountInfo, GlobalInfo {
        let sender = Signer::address_of(sig);

        accept(sig);

        let account_info = borrow_global_mut<AccountInfo>(sender);

        let token = Token { hdfs : copy hdfs };
        let token_id = compute_token_id(&token);

        assert(!Vector::contains<Token>(&account_info.tokens, &token), 10009);

        Vector::push_back<Token>(&mut account_info.tokens, token);        
       
        // emit event to global info 
        emit_minted_event(sender, copy token_id, copy hdfs);

        // emit event to current account
        Event::emit_event<MintedTokenEvent>(&mut account_info.minted_events, MintedTokenEvent { token_id, hdfs, minter:sender });       
    }   
    /*
    *   Get token index from id
    */
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

    fun emit_shared_token_event(sender: address, receiver: address, token_id: vector<u8>, metadata: vector<u8>) 
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
        assert(exists<AccountInfo>(receiver), 10003);

        let sender_info = borrow_global_mut<AccountInfo>(sender);

        // copy a token from sender
        let token = *Vector::borrow(&sender_info.tokens, index);
        let token_id = compute_token_id(&token);
        
        // get the account info from receiver
        let receiver_info = borrow_global_mut<AccountInfo>(receiver);

        // put the token to receiver account
        Vector::push_back<Token>(&mut receiver_info.tokens, token);
        
        // emit shared event to global info
        emit_shared_token_event(sender, receiver, copy token_id, copy metadata);
        
        // emit sent event to sender's account
        Event::emit_event<SentTokenEvent>(&mut borrow_global_mut<AccountInfo>(sender).sent_events, 
                                        SentTokenEvent { receiver, token_id: copy token_id, metadata: copy metadata });
        
        // Emit received event to receiver's account
        Event::emit_event<ReceivedTokenEvent>(&mut borrow_global_mut<AccountInfo>(receiver).received_events, 
                                        ReceivedTokenEvent {sender, token_id, metadata});
    }
    
}

}

