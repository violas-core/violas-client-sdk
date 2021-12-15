address 0x2 {

module NftStore {
    use Std::Signer;
    use Std::Vector;
    use Std::BCS;
    use Std::Errors;
    use Std::Event::{Self, EventHandle};
    use Std::Compare;
    use Std::FixedPoint32::{Self, FixedPoint32};
    use Std::Hash;
    use DiemFramework::VASP;
    use DiemFramework::Diem;
    use DiemFramework::DiemAccount;
    use 0x2::NonFungibleToken;
    
    const ADMIN_ACCOUNT_ADDRESS : address = @0x1122;

    const ENFT_STORE_HAS_BEEN_INITIALIZED: u64 = 10001;
    const ENFT_TYPE_HAS_BEEN_REGISTERED: u64 = 10002;

    struct MadeOrderEvent has drop, store {
        nft_token_id : vector<u8>,
        order_id : vector<u8>,
    }

    struct RevokedOrderEvent  has drop, store {
        nft_token_id : vector<u8>,
        order_id : vector<u8>,
    }

    struct TradedOrderEvent  has copy, drop, store {        
        order_id : vector<u8>,
        nft_token_id : vector<u8>,
        payer : address,
        payee : address,
        agent : address,
        price : u64,
        incentive : u64,        
    }

    struct SentSignedTxnEvent has drop , store {
        receiver_address: address,
        signed_txn : vector<u8>
    }

    struct ReceivedSignedTxnEvent has drop , store {
        sender_address: address,
        signed_txn : vector<u8>
    }

    struct Order has drop, store {
        price : u64,
        currency_code : vector<u8>,
        sale_incentive : FixedPoint32,
        provider : address,
        nft_token_id : vector<u8>,
    } 
    //
    // Order list held by admin
    //
    struct OrderList<T> has key, store {
        orders : vector<Order>,        
    }
    //
    //  Global configuration held by admin
    //
    struct Configuration has key, store {
        withdraw_cap: NonFungibleToken::WithdrawCapbility,
        provider_parent_address : address,
        sale_agent_parent_address: address,
    }
    //
    //  account info held by each customer account
    //
    struct Account<NFT> has key {
        made_order_events: EventHandle<MadeOrderEvent>,
        revoked_order_events: EventHandle<RevokedOrderEvent>,
        traded_order_events : EventHandle<TradedOrderEvent>,
        sent_trading_order_sig_events: EventHandle<SentSignedTxnEvent>,
        received_trading_order_sig_events: EventHandle<ReceivedSignedTxnEvent>,
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
    public fun get_admind_address() : address {
        ADMIN_ACCOUNT_ADDRESS
    }

    //
    //  Initialize Order Store by admin account
    //
    public fun initialize(
        sig: &signer, 
        provider_parent_address: address,
        sale_agent_parent_address: address) 
    {        
        check_admin_permission(sig);

        let sender = Signer::address_of(sig);
        assert(!exists<Configuration>(sender), Errors::already_published(ENFT_STORE_HAS_BEEN_INITIALIZED));

        move_to(sig, Configuration {
            withdraw_cap: NonFungibleToken::extract_opt_withdraw_capability(sig),
            provider_parent_address,
            sale_agent_parent_address
        });        
    }
    //
    // Register a new NFT type into Store
    //
    public fun register<NFT: store>(sig: &signer) {        
        check_admin_permission(sig);
        
        let sender = Signer::address_of(sig);
        assert(!exists<OrderList<NFT>>(sender), Errors::already_published(ENFT_TYPE_HAS_BEEN_REGISTERED));

        move_to(sig, 
            OrderList<NFT> {
            orders : Vector::empty<Order>()            
        });

        if(!NonFungibleToken::has_accepted<NFT>(sender)) {
            NonFungibleToken::accept<NFT>(sig);
        };        
    }
    //
    //  Accpet Account info
    //
    public fun accept<NFT: store>(sig: &signer) {
        let sender = Signer::address_of(sig);

        assert(exists<Account<NFT>>(sender), Errors::already_published(11111));

        move_to(sig, 
            Account<NFT> {
                made_order_events: Event::new_event_handle<MadeOrderEvent>(sig),
                revoked_order_events: Event::new_event_handle<RevokedOrderEvent>(sig),
                traded_order_events: Event::new_event_handle<TradedOrderEvent>(sig),
                sent_trading_order_sig_events: Event::new_event_handle<SentSignedTxnEvent>(sig),
                received_trading_order_sig_events: Event::new_event_handle<ReceivedSignedTxnEvent>(sig),
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
        price : u64, 
        sale_incentive : FixedPoint32, 
        nft_token_id: &vector<u8>) 
    acquires Account, OrderList {        
        
        let sender = Signer::address_of(sig);               
        
        // ensure that the provider is a child account of admin account
        // assert(VASP::parent_address(provider) == ADMIN_ACCOUNT_ADDRESS, 100005);
        assert(Diem::is_currency<Token>(), 100006);

        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);
        
        NonFungibleToken::transfer<NFT>(sig, get_admind_address(), nft_token_id, &b"make order to NFT Store");

        let currency_code = Diem::currency_code<Token>();
        let order = Order { price, currency_code, sale_incentive, provider: sender, nft_token_id: *nft_token_id };
        let order_id = compute_order_id(&order);

        Vector::push_back<Order>(&mut order_list.orders, order);
        
        if(!exists<Account<NFT>>(sender)) {
            accept<NFT>(sig);
        };

        let account = borrow_global_mut<Account<NFT>>(sender);
        
        Event::emit_event(&mut account.made_order_events, 
            MadeOrderEvent {
                nft_token_id: *nft_token_id,
                order_id
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
        let Order { price:_, currency_code:_, sale_incentive:_, provider:_, nft_token_id } = Vector::swap_remove(&mut order_list.orders, index);       

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
        sale_agent_sig : &signer,
        order_id : &vector<u8>) 
    acquires Account, OrderList, Configuration {
        let sender = Signer::address_of(sender_sig);
        let sale_agent = Signer::address_of(sale_agent_sig);
        let (ret, index) = find_order<NFT>(order_id);
        
        assert(ret == true, 10002);
        
        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);
        let order = Vector::borrow<Order>(&order_list.orders, index);        

        let balance = DiemAccount::balance<Token>(sender);
        
        assert(balance > order.price, 100003);
        
        let incentive_amount = FixedPoint32::multiply_u64(order.price, *&order.sale_incentive);
        
        let sender_withdraw_cap = DiemAccount::extract_withdraw_capability(sender_sig);
        
        // Pay to sale agent
        DiemAccount::pay_from<Token>(&sender_withdraw_cap, 
                                    sale_agent, 
                                    incentive_amount, 
                                    b"", 
                                    b"");
        
        // Pay to asset provider
        DiemAccount::pay_from<Token>(&sender_withdraw_cap, 
                                    order.provider, 
                                    order.price - incentive_amount, 
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

        // Emit traded order event
        {
            // Make a traded order event      
            let traded_order_event = TradedOrderEvent {
                    order_id: *order_id,
                    nft_token_id : *&order.nft_token_id,
                    payer : order.provider,
                    payee : sender,
                    agent : sale_agent,
                    price : order.price,
                    incentive : FixedPoint32::get_raw_value(*&order.sale_incentive),
                };
            
            // Emit trade order events to sender account
            let sender_account = borrow_global_mut<Account<NFT>>(sender);
            Event::emit_event(&mut sender_account.traded_order_events, 
                copy traded_order_event);
            
            // Emit trade order events to provider account
            let provider_account = borrow_global_mut<Account<NFT>>(order.provider);
            Event::emit_event(&mut provider_account.traded_order_events, 
                copy traded_order_event);
            
            // Check if the account info exists under sale agent account
            if(!exists<Account<NFT>>(sale_agent))
                accept<NFT>(sale_agent_sig);

            // Emit trade order events to sale agent account
            let sale_agent_account = borrow_global_mut<Account<NFT>>(sale_agent);
            Event::emit_event(&mut sale_agent_account.traded_order_events, 
                traded_order_event);
        };
        
        // Destory order
        let Order { price:_, currency_code:_, sale_incentive:_, provider:_, nft_token_id:_ } = Vector::swap_remove(&mut order_list.orders, index);
    }


    //
    fun send_signed_trading_order_txn<NFT: store>(
        sender_sig : &signer, 
        sale_agent_address: address, 
        signed_trading_order_txn: vector<u8>) 
    acquires Account, Configuration {        
        //
        let sender_address = Signer::address_of(sender_sig);
        
        let configuration = borrow_global<Configuration>(ADMIN_ACCOUNT_ADDRESS);
        
        assert(VASP::parent_address(sale_agent_address) == configuration.sale_agent_parent_address, 11000);

        //Emit a signed tarding order event
        let sale_agent_account = borrow_global_mut<Account<NFT>>(sale_agent_address);
            Event::emit_event(&mut sale_agent_account.received_trading_order_sig_events, 
                ReceivedSignedTxnEvent {
                    sender_address,
                    signed_txn:  signed_trading_order_txn
                });
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