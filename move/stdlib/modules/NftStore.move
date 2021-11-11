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
    
    const ENFT_STORE_HAS_BEEN_INITIALIZED: u64 = 10001;
    const ENFT_TYPE_HAS_BEEN_REGISTERED: u64 = 10002;

    struct MadeOrderEvent has drop, store {
        nft_token_id : vector<u8>,
        order_id : vector<u8>,
    }

    struct RevokedOrderEvent  has drop, store {

    }

    struct Order has store {
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
        //amount : u64,
    }
    //
    //  account info held by each customer account
    //
    struct Account<NFT> {
        made_events: EventHandle<MadeOrderEvent>,
        revoked_events: EventHandle<RevokedOrderEvent>,
    }

    const ADMIN_ACCOUNT_ADDRESS : address = @1122;

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
    public fun initialize(sig: &signer) {        
        check_admin_permission(sig);

        let sender = Signer::address_of(sig);
        assert(exists<Configuration>(sender), Errors::already_published(ENFT_STORE_HAS_BEEN_INITIALIZED));

        move_to(sig, Configuration {
            withdraw_cap: NonFungibleToken::extract_opt_withdraw_capability(sig)
        });        
    }
    //
    // Register a new NFT store with NFT type
    //
    public fun register<NFT: store>(sig: &signer) {        
        check_admin_permission(sig);
        
        let sender = Signer::address_of(sig);
        assert(exists<OrderList<NFT>>(sender), Errors::already_published(ENFT_TYPE_HAS_BEEN_REGISTERED));

        move_to(sig, 
            OrderList<NFT> {
            orders : Vector::empty<Order>()            
        });

        NonFungibleToken::accept<NFT>(sig);
    }
    //
    //  Accpet Account info
    //
    public fun accept<NFT: store>(sig: &signer) {
        
        assert(exists<Account<NFT>>(sig), Errors::already_published(11111));

        move_to(sig, 
            Account<NFT> {
                made_events: Event::new_event_handle<MadeOrderEvent>(),
                revoked_events: Event::new_event_handle<RevokedOrderEvent>(),
            });
    }

    //
    //
    //
    public fun make_order<NFT: store, Token>(
        sig : &signer, 
        price : u64, 
        sale_incentive : FixedPoint32, 
        nft_token_id: &vector<u8>) 
    acquires OrderList {        
        
        let provider = Signer::address_of(sig);               
        
        // ensure that the provider is a child account of admin account
        // assert(VASP::parent_address(provider) == ADMIN_ACCOUNT_ADDRESS, 100005);
        assert(Diem::is_currency<Token>(), 100006);

        let order_list = borrow_global_mut<OrderList<NFT>>(ADMIN_ACCOUNT_ADDRESS);
        
        NonFungibleToken::transfer<NFT>(sig, get_admind_address(), nft_token_id, &b"make order to NFT Store");

        let currency_code = Diem::currency_code<Token>();
        let order = Order { price, currency_code, sale_incentive, provider, nft_token_id: *nft_token_id };
        let order_id = compute_order_id(&order);

        Vector::push_back<Order>(&mut order_list.orders, order);
        
        if(!exists<Account<NFT>>(sig)) {
            accept(sig);
        };

        let account = borrow_global_mut<Account<NFT>>(provider);
        
        Event::emit_event(&mut account.made_events, 
            MadeOrderEvent {
                nft_token_id: *nft_token_id,
                order_id
            });
    }

    public fun revoke_order<NFT: store>(sig: &signer, order_id: vector<u8>)
    acquires OrderList, Configuration {        
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
        let Order { price:_, currency_code:_, sale_incentive:_, provider:_, nft_token_id:_ } = Vector::swap_remove(&mut order_list.orders, index);       
    }

    //
    //
    //
    public fun trade_order<Token, T: store>(
        sender_sig : &signer,
        sale_agent_sig : &signer,
        order_id : &vector<u8>) 
    acquires OrderList {
        let sender = Signer::address_of(sender_sig);
        let sale_agent = Signer::address_of(sale_agent_sig);
        let (ret, index) = find_order<T>(order_id);
        
        assert(ret == true, 10002);
        
        let order_list = borrow_global_mut<OrderList<T>>(ADMIN_ACCOUNT_ADDRESS);
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

        // Move the order to sender account
        let order = Vector::swap_remove<Order>(&mut order_list.orders, index);

        if(exists<OrderList<T>>(sender)) {
            let sender_order_list = borrow_global_mut<OrderList<T>>(sender);
            Vector::push_back<Order>(&mut sender_order_list.orders, order);
        } else
            move_to<OrderList<T>>(sender_sig, OrderList { orders : Vector::singleton<Order>(order)});

        //emit events
        
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
        let order_bcs = BCS::to_bytes<Token>(order);
        Hash::sha3_256(order_bcs)        
    }

    // public fun fetch_order() : Order {

    // }
}  

}