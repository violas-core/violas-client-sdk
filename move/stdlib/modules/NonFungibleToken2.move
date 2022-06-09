address 0x2 {
// 
// NFT implementation with Move lanaguage
// reference ERC721 on http://erc721.org/
// 
module NonFungibleToken2 {
    use Std::BCS;
    use Std::Compare;
    use Std::Errors;
    use Std::Event::{Self, EventHandle};
    use Std::Hash;    
    use Std::Option::{Self, Option};
    use Std::Signer;
    use Std::Vector;
    use Std::Map::{Self, Map};    

    const NFT_PUBLISHER: address = @0xA550C18;   // Violas root account
    const EPAYEE_CANT_ACCEPT_NFT: u64 = 1001;
    const ESENDER_HAS_ACCEPTED_NFT_TYPE: u64 = 1002;
    const ENFT_TOKEN_HAS_ALREADY_EXISTED: u64 = 1003;
    const ENFT_TOKEN_HAS_NOT_EXISTED: u64 = 1004;    
    const EACCOUNT: u64 = 10001;
    
    struct MintedEvent has drop, store {
        token_id: vector<u8>,
        receiver: address,
    }

    struct BurnedEvent has drop, store {
        token_id: vector<u8>,
    }

    struct SentEvent has drop, store {
        token_id: vector<u8>,
        payee: address,
        metadata: vector<u8>,
    }

    struct ReceivedEvent has drop, store {
        token_id: vector<u8>,
        payer: address,
        metadata: vector<u8>,
    }

    struct TransferredEvent has drop, store {
        token_id: vector<u8>,
        payee: address,
        payer: address,
        metadata: vector<u8>,
    }
    //
    //  NFT global infomation held by the administrator account
    //
    struct Configuration<Token> has key {
        limited: bool,
        total: u64,
        amount: u64,
        admin_address: address, // administrator's address
        symbol : vector<u8>,       // NFT symbol
        owners: Map<vector<u8>, address>,  // token id maps to owner's address        
        // Global events
        minted_events: EventHandle<MintedEvent>,
        burned_events: EventHandle<BurnedEvent>,
        transferred_events: EventHandle<TransferredEvent>
    }   

    struct RegisteredNfts has key {
        nft_syms : vector<vector<u8>>,
    }

    struct WithdrawCapability has store {
        account_address : address,
    }
    //
    //  Account struct held by the account
    //
    struct Account has key {
        opt_withdraw_cap : Option<WithdrawCapability>,
    }
    //
    //  Non Fungiable Token resource held by the account
    //
    struct NFT<Token> has key {        
        tokens: vector<Token>,                          // store all tokens
        sent_events: EventHandle<SentEvent>,            // the sent events
        received_events: EventHandle<ReceivedEvent>,    // the received events
    }
  
    //
    //  Increase 1 to NFT amount
    //
    fun increase_nft_amount<Token: store>() 
    acquires Configuration {
        let info = borrow_global_mut<Configuration<Token>>(NFT_PUBLISHER);
        
        * (&mut info.amount) = info.amount + 1;
        
        assert(info.amount <= info.total, 1000);
    }
    //
    //  Decrease 1 NFT amount 
    //
    fun decrease_nft_amount<Token: store>() 
    acquires Configuration {
        let info = borrow_global_mut<Configuration<Token>>(NFT_PUBLISHER);
        
        if (info.amount != 0) {
            * (&mut info.amount) = info.amount - 1;
        }
    }    
    //
    //  check if the address of admin 
    //
    fun check_admin_permission<Token: store>(account_address: address)
    acquires Configuration {
        let info = borrow_global<Configuration<Token>>(NFT_PUBLISHER);

        assert(info.admin_address == account_address, 10001);
    }
    //
    //
    //
    public fun initialize(sig: &signer) {
        let sender = Signer::address_of(sig);
        assert(sender == NFT_PUBLISHER, 8000);
        
        assert(!exists<RegisteredNfts>(sender), 8001);

        move_to<RegisteredNfts>(sig, RegisteredNfts{ nft_syms : Vector::empty<vector<u8>>() });
    }

    fun add_nft_sym(symbol : vector<u8> )
    acquires RegisteredNfts {
        
        assert(exists<RegisteredNfts>(NFT_PUBLISHER), 8001);

        let registered_nfts = borrow_global_mut<RegisteredNfts>(NFT_PUBLISHER);

        Vector::push_back<vector<u8>>(&mut registered_nfts.nft_syms, symbol);
    }
    //
    //  Get NFT symbol
    //
    public fun get_symbol<Token : store>() : vector<u8>
    acquires Configuration {
        let configuration = borrow_global<Configuration<Token>>(NFT_PUBLISHER);

        *&configuration.symbol
    }
    //
    //
    //
    public fun register<Token: store>(sig: &signer, limited: bool, total: u64, admin_address: address, symbol: vector<u8>)
    acquires RegisteredNfts {
        let sender = Signer::address_of(sig);
        assert(sender == NFT_PUBLISHER, 8000);

        let info = Configuration<Token> {
            limited: limited,
            total: total,
            amount: 0,
            admin_address,
            symbol: copy symbol,
            owners:  Map::empty<vector<u8>, address>(),
            minted_events: Event::new_event_handle<MintedEvent>(sig),
            burned_events: Event::new_event_handle<BurnedEvent>(sig),
            transferred_events : Event::new_event_handle<TransferredEvent>(sig),
        };

        move_to<Configuration<Token>>(sig, info);
        
        // add NFT symbol to the registered symbol list
        add_nft_sym(symbol);
    }
    
    //
    //  Accept NFT tokens
    //
    public fun accept<Token: store>(sig: &signer) {
        let sender = Signer::address_of(sig);
                
        assert(!exists<NFT<Token>>(sender), Errors::already_published(ESENDER_HAS_ACCEPTED_NFT_TYPE));               
        
        move_to<NFT<Token>>(sig, 
            NFT<Token> {
                tokens: Vector::empty<Token>(),
                sent_events: Event::new_event_handle<SentEvent>(sig),
                received_events: Event::new_event_handle<ReceivedEvent>(sig),
            });
        
        make_account(sig);        
    }
    //
    // Has accepted
    //
    public fun has_accepted<Token: store>(sender: address) : bool {
        exists<NFT<Token>>(sender)
    }
    //
    //
    //
    public fun make_account(sig: &signer) {
        let sender = Signer::address_of(sig);
        
        if(!exists<Account>(sender)){
            move_to<Account>(sig, 
                Account { opt_withdraw_cap: Option::some(WithdrawCapability { account_address: sender }) });
        };
    }
    //
    //  Get the number of balance for Token
    //
    public fun balance<Token : store>(sig: &signer) : u64 
    acquires NFT {
        let sender = Signer::address_of(sig);

        let account = borrow_global<NFT<Token>>(sender);
        
        Vector::length<Token>(&account.tokens)
    }
    
    //
    //
    //
    public fun compute_token_id<Token>(token: &Token) : vector<u8> {
        let token_bcs = BCS::to_bytes<Token>(token);
        let token_id = Hash::sha3_256(token_bcs);

        token_id
    }
    
    //
    //  Get NFT token index by token id
    //  if returned index is equal to the length of vector that means faild to get index
    //
    fun get_token_index<Token: store>(tokens: &vector<Token>, token_id: &vector<u8>) : u64 {

        let length = Vector::length<Token>(tokens);
        let i: u64 = 0;
        
        while(i < length) {
            let token = Vector::borrow<Token>(tokens, i);
            if( Compare::cmp_bcs_bytes(&compute_token_id(token), token_id) == 0 ) {
                break
            } else {
                i = i + 1;
            }
        };

        i
    }
    
    //
    //  Get NFT token from an account
    //
    fun get_nft_token<Token: store>(account: &signer, token_id: &vector<u8>): Token 
    acquires NFT {
        let sender = Signer::address_of(account);
        assert(exists<NFT<Token>>(sender), 8006);
        //assert(!exists<TokenLock<Token>>(sender), 8007);
        //Self::lock<Token>(account);

        let nft = borrow_global_mut<NFT<Token>>(sender);    
        let length = Vector::length<Token>(&nft.tokens);

        let index = get_token_index<Token>(&nft.tokens, token_id);
        assert(index < length, 10001);

        if (index != length-1)
            Vector::swap_remove<Token>(&mut nft.tokens, index)
        else
            Vector::pop_back<Token>(&mut nft.tokens)        
    }
       
    //
    //  get token owner by token id
    //
    public fun owner<Token:key+store>(token_id: &vector<u8>): Option<address>
    acquires Configuration {
        let info = borrow_global<Configuration<Token>>(NFT_PUBLISHER);

        let (index, found) = Map::find<vector<u8>, address>(&info.owners, token_id);
        if( found ) {
            let(_, owner) = Map::borrow(&info.owners, index);            
                    
            Option::some(*owner)
        } else {
            Option::none()
        }
        
    }
    
    ///
    /// Mint a NFT to a receiver
    /// 
    public fun mint<Token: store>(sig: &signer, receiver: address, token: Token) : bool
    acquires NFT, Configuration  {

        let sender = Signer::address_of(sig);

        check_admin_permission<Token>(sender);

        // The receiver must has called method 'accept_token' previously
        assert(exists<NFT<Token>>(receiver), Errors::not_published(EPAYEE_CANT_ACCEPT_NFT));
        
        let token_id = compute_token_id<Token>(&token);                

        let info = borrow_global_mut<Configuration<Token>>(NFT_PUBLISHER);
        
        // Insert to global map
        let ret = Map::insert(&mut info.owners, copy token_id, receiver);
        
        // Abort if token id has already existed            
        assert( ret, Errors::invalid_argument(ENFT_TOKEN_HAS_ALREADY_EXISTED) );  
        
        // Emit a minted event
        Event::emit_event(&mut info.minted_events, MintedEvent{ token_id, receiver });
        
        //  Increment NFT amount
        increase_nft_amount<Token>();
        //
        //  Deposite NFT to receiver
        //
        let receiver_token_ref_mut = borrow_global_mut<NFT<Token>>(receiver);    
                
        Vector::push_back<Token>(&mut receiver_token_ref_mut.tokens, token);        
        
        true
    }
    
    //
    //  Burn a NFT token
    //
    public fun burn<Token: drop+store>(sig: &signer, token_id: &vector<u8>)
    acquires NFT, Configuration  {
    
        let sender = Signer::address_of(sig);
                
        check_admin_permission<Token>(sender);        

        // // Erase all owners by token id        
        // // let ret = Map::erase<vector<u8>, vector<address>>(&mut info.owners, token_id);
        // // assert(ret, Errors::invalid_argument(ENFT_TOKEN_HAS_NOT_EXISTED));
        
        // drop token by token id
        let _token = get_nft_token<Token>(sig, token_id);

        decrease_nft_amount<Token>();
        //
        // Emit a burned event
        //                
        let info = borrow_global_mut<Configuration<Token>>(NFT_PUBLISHER);
        
        Event::emit_event(&mut info.burned_events, BurnedEvent{ token_id: *token_id });
    }
    //
    //  pay NFT with withdraw capabilidy
    //
    public fun pay_from<Token: store>(
        cap: &WithdrawCapability, 
        receiver: address, 
        token_id: &vector<u8>, 
        metadata: &vector<u8>)
    acquires NFT, Configuration {
        let sender = cap.account_address;

        let token = withdraw<Token>(cap, sender, *token_id, metadata);
        deposite<Token>(sender, receiver, token, metadata);

        // Update owner of token id
        let info = borrow_global_mut<Configuration<Token>>(NFT_PUBLISHER);
        
        let (index, found) = Map::find(&info.owners, token_id);
        if(found) {
            let (_, value) = Map::borrow_mut(&mut info.owners, index);
            *value = receiver;
        } else {
            abort(8004)
        };

        Event::emit_event(&mut info.transferred_events, TransferredEvent {token_id: *token_id, payer: sender, payee: receiver, metadata: *metadata});
    }
    //
    //  Transfer a NFT token with token id
    //
    public fun transfer<Token: store>(sig: &signer, receiver: address, token_id: &vector<u8>, metadata: &vector<u8>) 
    acquires Account, NFT, Configuration {
        let sender = Signer::address_of(sig);
        assert(sender != receiver, 10010);
        
        let withdraw_cap = extract_opt_withdraw_capability(sig);        
        
        pay_from<Token>(&withdraw_cap, receiver, token_id, metadata);
                
        restore_opt_withdraw_capability(withdraw_cap);
    }
    
    //
    //  Transfer a NFT token with index
    //
    public fun transfer_by_index<Token: store>(sig: &signer, receiver: address, index: u64, metadata: &vector<u8>) 
    acquires Account, NFT, Configuration {
        let sender = Signer::address_of(sig);

        assert(exists<NFT<Token>>(receiver), 8002);                                
        //assert(!exists<TokenLock<Token>>(sender), 8004);

        let sender_nft = borrow_global_mut<NFT<Token>>(sender);
        let length = Vector::length<Token>(&sender_nft.tokens);
        
        // Ensure the index is valid 
        assert(index < length, 8003);

        let token = Vector::borrow(&sender_nft.tokens, index);
                
        let token_id = compute_token_id(token);
                
        transfer<Token>(sig, receiver, &token_id, metadata);
    }

    fun withdraw<Token: store>(cap: &WithdrawCapability, receiver: address, token_id : vector<u8>, metadata: &vector<u8>) : Token
    acquires NFT {
        let sender = cap.account_address;
        
        let sender_token_ref_mut = borrow_global_mut<NFT<Token>>(sender);
        let index = get_token_index(&sender_token_ref_mut.tokens, &token_id);

        let sender_nft = borrow_global_mut<NFT<Token>>(sender);
        let length = Vector::length<Token>(&sender_nft.tokens);
        
        // Ensure the index is valid 
        assert(index < length, 8003);
        
        // Get token from sender
        if (index < length - 1) //swap element to back
            Vector::swap<Token>(&mut sender_nft.tokens, index, length-1); 

        let token = Vector::pop_back(&mut sender_nft.tokens);

        // Emit sent event
        let sender_account =  borrow_global_mut<NFT<Token>>(sender);
        Event::emit_event(&mut sender_account.sent_events, SentEvent{ token_id: copy token_id, payee: receiver, metadata: *metadata });

        token
    } 
    //
    // Deposite a token to receiver
    //
    fun deposite<Token: store>(sender: address, receiver: address, token : Token, metadata: &vector<u8>) 
    acquires NFT {
        let token_id = compute_token_id(&token);
        // Deposite token to receiver         
        let nft_receiver =  borrow_global_mut<NFT<Token>>(receiver);
        Vector::push_back<Token>(&mut nft_receiver.tokens, token);
        
        // Emit received event
        let receiver_account =  borrow_global_mut<NFT<Token>>(receiver);        
        Event::emit_event(&mut receiver_account.received_events, ReceivedEvent{ token_id, payer: sender, metadata: *metadata });
    }

    public fun extract_opt_withdraw_capability(sig: &signer) : WithdrawCapability
    acquires Account {
        let sender = Signer::address_of(sig);
        assert(exists<Account>(sender), Errors::not_published(EACCOUNT));

        let account = borrow_global_mut<Account>(sender);
        assert(Option::is_some(&account.opt_withdraw_cap), Errors::not_published(EACCOUNT+1));

        Option::extract(&mut account.opt_withdraw_cap)
    }

    public fun restore_opt_withdraw_capability(cap : WithdrawCapability) 
    acquires Account {
        assert(exists<Account>(cap.account_address), Errors::not_published(EACCOUNT));
        
        let account = borrow_global_mut<Account>(cap.account_address);
        assert(Option::is_none(&account.opt_withdraw_cap), Errors::not_published(EACCOUNT+1));

        Option::fill(&mut account.opt_withdraw_cap, cap)
    }
}
}
