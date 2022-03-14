address 0x2 {

module NftStore2 {
    use Std::Signer;
    use Std::Vector;
    use Std::BCS;
    use Std::Errors;
    use Std::Event::{Self, EventHandle};
    use Std::Compare;
    use Std::FixedPoint32::{Self, FixedPoint32};
    use Std::Hash;
    //use DiemFramework::VASP;
    use DiemFramework::Diem;
    use DiemFramework::DiemAccount;
    use DiemFramework::DiemTimestamp;
    
    use 0x2::NonFungibleToken;
    
    const ADMIN_ACCOUNT_ADDRESS : address = @0x1122;

    const ENFT_STORE_HAS_BEEN_INITIALIZED: u64 = 10001;
    const ENFT_TYPE_HAS_BEEN_REGISTERED: u64 = 10002;
    //
    //  All events declaration
    //
    struct MadeOrderEvent has drop, store {
        order_id : vector<u8>,
        nft_token_id : vector<u8>,        
        price: u64,
        currency_code: vector<u8>,        
    }

    struct RevokedOrderEvent  has drop, store {
        order_id : vector<u8>,
        nft_token_id : vector<u8>,        
    }

    struct TradedOrderEvent  has copy, drop, store {        
        order_id : vector<u8>,
        nft_token_id : vector<u8>,
        payer : address,
        payee : address,        
        price : u64,
        currency: vector<u8>,
        fee_rate: FixedPoint32,
    }
    
    struct SoldOrderEvent has drop, store {
        order_id : vector<u8>,
        nft_token_id : vector<u8>,
        buyer: address,
        price : u64,        
        currency: vector<u8>,
        fee_rate: FixedPoint32,
    }

    struct BoughtOrderEvent has drop, store {
        order_id : vector<u8>,
        nft_token_id : vector<u8>,
        seller: address,
        price : u64,
        currency: vector<u8>,
    }

    struct Order has store {
        nft_token_id : vector<u8>,
        price : u64,
        currency_code : vector<u8>,        
        provider : address,        
        timestamp : u64
    } 
    //
    // Order list held holden by admin account
    //
    struct OrderList<NFT> has key {
        orders : vector<Order>,
        fee_rate : FixedPoint32,
        traded_order_events : EventHandle<TradedOrderEvent>,
    }
    //
    //  Global configuration holden by admin account
    //
    struct Configuration has key {
        withdraw_cap: NonFungibleToken::WithdrawCapbility,
        nft_syms :vector<vector<u8>>,       // NFT symbole list
    }
    //
    //  account info holden by each customer account
    //
    struct Account<NFT> has key, store {
        made_order_events: EventHandle<MadeOrderEvent>,
        revoked_order_events: EventHandle<RevokedOrderEvent>,
        sold_order_events: EventHandle<SoldOrderEvent>,
        bought_order_events: EventHandle<BoughtOrderEvent>,
    }    

    //
    //  Ensure that the signer is administor account
    //
    public fun check_admin_permission(sig : &signer) {
        assert(Signer::address_of(sig) == ADMIN_ACCOUNT_ADDRESS, 1000);
    }
    //
    //
    //
    public fun get_admin_address() : address {
        ADMIN_ACCOUNT_ADDRESS
    }

    //
    //  Initialize Order Store by admin account
    //
    public fun initialize( sig: &signer ) 
    {
        let sender = Signer::address_of(sig);

        check_admin_permission(sig);        

        NonFungibleToken::make_account(sig);

        assert(!exists<Configuration>(sender), Errors::already_published(ENFT_STORE_HAS_BEEN_INITIALIZED));

        move_to(sig, Configuration {
            withdraw_cap: NonFungibleToken::extract_opt_withdraw_capability(sig),
            nft_syms: Vector::empty(),
        });
    }
    //
    // Register a new NFT type into Store
    //
    public fun register<NFT: store>(sig: &signer, fee_rate :FixedPoint32) 
    acquires Configuration {        
        check_admin_permission(sig);
        
        let sender = Signer::address_of(sig);
        assert(!exists<OrderList<NFT>>(sender), Errors::already_published(ENFT_TYPE_HAS_BEEN_REGISTERED));

        move_to(sig, 
            OrderList<NFT> {
            orders : Vector::empty<Order>(),
            fee_rate,
            traded_order_events: Event::new_event_handle<TradedOrderEvent>(sig),
        });

        if(!NonFungibleToken::has_accepted<NFT>(sender)) {
            NonFungibleToken::accept<NFT>(sig);
        };

        let config = borrow_global_mut<Configuration>(ADMIN_ACCOUNT_ADDRESS);
        let nft_symbol = NonFungibleToken::get_symbol<NFT>();

        Vector::push_back(&mut config.nft_syms, nft_symbol);
    }
    //
    //  get all registered NFT symbol list
    //
    public fun get_nft_symbols() : vector<vector<u8>>
    acquires Configuration {
        let configuration = borrow_global<Configuration>(ADMIN_ACCOUNT_ADDRESS);

        *&configuration.nft_syms
    }
    //
    //  Accpet Account info
    //
    public fun accept<NFT: store>(sig: &signer) {
        let sender = Signer::address_of(sig);

        assert(!exists<Account<NFT>>(sender), (11111)); //Errors::already_published

        move_to(sig, 
            Account<NFT> {
                made_order_events: Event::new_event_handle<MadeOrderEvent>(sig),
                revoked_order_events: Event::new_event_handle<RevokedOrderEvent>(sig),
                sold_order_events: Event::new_event_handle<SoldOrderEvent>(sig),
                bought_order_events: Event::new_event_handle<BoughtOrderEvent>(sig),
            });
        
        if(!NonFungibleToken::has_accepted<NFT>(sender)) {
            NonFungibleToken::accept<NFT>(sig);
        }; 
    }

    //
    //
    //
    public fun make_order<NFT: store, Token>(
        sig : &signer, 
        nft_token_id: &vector<u8>,
        price : u64) 
    acquires Account, OrderList {
        
        let sender = Signer::address_of(sig);
        
        // ensure that the provider is a child account of admin account
        // assert(VASP::parent_address(provider) == ADMIN_ACCOUNT_ADDRESS, 100005);
        assert(Diem::is_currency<Token>(), 100006);

        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);
        
        NonFungibleToken::transfer<NFT>(sig, get_admin_address(), nft_token_id, &b"make order to NFT Store");

        let currency_code = Diem::currency_code<Token>();        
        let order = Order { price, 
                            currency_code: copy currency_code,                             
                            provider: sender, 
                            nft_token_id: *nft_token_id,
                            timestamp: DiemTimestamp::now_seconds() };
        let order_id = compute_order_id(&order);

        Vector::push_back<Order>(&mut order_list.orders, order);
        
        if(!exists<Account<NFT>>(sender)) {
            accept<NFT>(sig);
        };

        let account = borrow_global_mut<Account<NFT>>(sender);
        
        Event::emit_event(&mut account.made_order_events, 
            MadeOrderEvent {
                order_id,
                nft_token_id: *nft_token_id,                
                price,
                currency_code,                
            });
    }
    //
    //  Revoke an order
    //  note : the address of signer must be same as the address of provider of order
    //
    public fun revoke_order<NFT: store>(sig: &signer, order_id: vector<u8>)
    acquires Account, OrderList, Configuration {        
        let sender = Signer::address_of(sig);

        let (ret, index) = find_order<NFT>(&order_id);
        assert(ret == true, 10003);  

        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);                    

        {            
            let order = Vector::borrow(&order_list.orders, index);
        
            // Make sure the provider of order is the same as sender
            assert(order.provider == sender, 10004);

            let configuration = borrow_global<Configuration>(ADMIN_ACCOUNT_ADDRESS);
                        
            NonFungibleToken::pay_from<NFT>(&configuration.withdraw_cap, order.provider, &order.nft_token_id, &b"revoke order from NFT store" );
        };
                
        // Destory order
        let Order { price:_, currency_code:_, provider:_, nft_token_id, timestamp:_ } = Vector::swap_remove(&mut order_list.orders, index);       
        
        let account = borrow_global_mut<Account<NFT>>(sender);
        Event::emit_event(&mut account.revoked_order_events, 
            RevokedOrderEvent {
                nft_token_id,
                order_id
            });        
    }

    //
    //  Trade NFT order with specified Token
    //
    public fun trade_order<NFT: store, Token>(
        sender_sig : &signer,        
        order_id : &vector<u8>) 
    acquires Account, OrderList, Configuration {
        let sender = Signer::address_of(sender_sig);        
        let (ret, index) = find_order<NFT>(order_id);
        
        assert(ret == true, 10002);
        
        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);
        let order = Vector::borrow<Order>(&order_list.orders, index);        

        let balance = DiemAccount::balance<Token>(sender);
        
        assert(balance > order.price, 10003);
                
        let sender_withdraw_cap = DiemAccount::extract_withdraw_capability(sender_sig);
        
        let fee = FixedPoint32::multiply_u64(order.price, *&order_list.fee_rate);
        let revenue = order.price - fee;
        
        // Pay to Store
        DiemAccount::pay_from<Token>(&sender_withdraw_cap, 
                                    get_admin_address(), 
                                    fee, 
                                    b"", 
                                    b"");

        // Pay to NFT provider
        DiemAccount::pay_from<Token>(&sender_withdraw_cap, 
                                    order.provider, 
                                    revenue,
                                    b"", 
                                    b"");

        DiemAccount::restore_withdraw_capability(sender_withdraw_cap);
        
        // Extract order from the order list
        let order = Vector::swap_remove<Order>(&mut order_list.orders, index);

        // Transfer the NFT from admin account to sender account
        {
            if(!NonFungibleToken::has_accepted<NFT>(sender)) {
                NonFungibleToken::accept<NFT>(sender_sig);
            };

            let configuration = borrow_global<Configuration>(ADMIN_ACCOUNT_ADDRESS);

            NonFungibleToken::pay_from<NFT>(&configuration.withdraw_cap, sender, &order.nft_token_id, &b"transfer a NFT from NFT store to sender" );            
        };
        //
        // Emit all event
        //
        {
            // Emit a traded order event to global storage                             
            Event::emit_event(
                &mut order_list.traded_order_events,
                TradedOrderEvent {
                    order_id: *order_id,
                    nft_token_id : *&order.nft_token_id,
                    payer : order.provider,
                    payee : sender,                    
                    price : order.price,
                    currency: *&order.currency_code,
                    fee_rate: *&order_list.fee_rate,
                });

            //
            // Emit sold order event to seller account
            //
            if(!exists<Account<NFT>>(sender))
                accept<NFT>(sender_sig);

            let seller_account = borrow_global_mut<Account<NFT>>(order.provider);
            Event::emit_event(
                &mut seller_account.sold_order_events, 
                SoldOrderEvent {
                    order_id : *order_id,
                    nft_token_id : *&order.nft_token_id,
                    buyer: sender,
                    price : order.price,                    
                    currency: *&order.currency_code,
                    fee_rate: *&order_list.fee_rate,
                });
            //            
            // Emit trade order events to buyer account
            //
            let buyer_account = borrow_global_mut<Account<NFT>>(sender);
            Event::emit_event(
                &mut buyer_account.bought_order_events, 
                BoughtOrderEvent {
                    order_id : *order_id,
                    nft_token_id : *&order.nft_token_id,
                    seller: order.provider,
                    price : order.price,
                    currency: *&order.currency_code,
                });            
        };
        
        // Destory order
        let Order { price:_, currency_code:_, provider:_, nft_token_id:_, timestamp:_ } = order;
    }
    

    //
    // Find the index of order from the order list
    // order_id : the sha3-256 hash of order
    //
    fun find_order<T: store>(order_id : &vector<u8>) : (bool, u64) 
    acquires OrderList {
        let order_list = borrow_global<OrderList<T>>(ADMIN_ACCOUNT_ADDRESS);

        let i = 0;
        let len = Vector::length(&order_list.orders);
        while (i < len) {
            let order = Vector::borrow(&order_list.orders, i);
            let hash = Hash::sha3_256(BCS::to_bytes(order));            

            if ( Compare::cmp_bcs_bytes(&hash, order_id) == 0) 
                return (true, i);
            
            i = i + 1;
        };

        (false, 0)
    }
    //
    //  Compute order id
    //
    fun compute_order_id(order: &Order) : vector<u8> {
        let order_bcs = BCS::to_bytes<Order>(order);
        Hash::sha3_256(order_bcs)        
    }

    // public fun fetch_order() : Order {

    // }
}  

}