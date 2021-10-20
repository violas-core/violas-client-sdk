address 0x2 {

module OrderStore {
    use Std::Signer;
    use Std::Vector;
    use Std::BCS;
    use Std::Compare;
    use Std::FixedPoint32::{Self, FixedPoint32};
    use Std::Hash;
    use DiemFramework::VASP;
    use DiemFramework::DiemAccount;

    struct Order has store {
        price : u64,
        sale_incentive : FixedPoint32,
        provider : address,
    } 

    struct OrderList has key, store {
        orders : vector<Order>,
    }

    const ADMIN_ACCOUNT_ADDRESS : address = @1122;

    //
    //  Ensure that the signer is administor account
    //
    public fun check_admin_permission(sig : &signer) {
        assert(Signer::address_of(sig) == ADMIN_ACCOUNT_ADDRESS, 1000);
    }

    //
    //  Initialize Order Store by admin account
    //
    public fun initialize(sig: &signer) {
        
        check_admin_permission(sig);

        move_to(sig, 
            OrderList {
            orders : Vector::empty<Order>(),            
        });
    }
    //
    //
    //
    public fun make_order(sig : &signer, price : u64, sale_incentive : FixedPoint32) 
    acquires OrderList {        
        let provider = Signer::address_of(sig);
        
        // ensure that the provider is a child account of admin account
        assert(VASP::parent_address(provider) == ADMIN_ACCOUNT_ADDRESS, 100005);

        let order_list = borrow_global_mut<OrderList>(ADMIN_ACCOUNT_ADDRESS);

        Vector::push_back<Order>(&mut order_list.orders, 
                                Order { price, sale_incentive, provider } );
    }

    public fun withdraw_order(sig: &signer, order_id: vector<u8>)
    acquires OrderList {
        let provider = Signer::address_of(sig);

        let (ret, index) = find_order(&order_id);
        assert(ret == true, 10003);  

        let order_list = borrow_global_mut<OrderList>(ADMIN_ACCOUNT_ADDRESS);                    

        let order = Vector::borrow(&order_list.orders, index);
        assert(order.provider == provider, 10004);
        
        // Destory order
        let Order { price : _price, sale_incentive:_, provider:_ } = Vector::swap_remove(&mut order_list.orders, index);    
    }

    //
    //
    //
    public fun take_order<Token>(
                            sig : &signer,
                            order_id : &vector<u8>) 
    acquires OrderList {
        let sender = Signer::address_of(sig);
        let sale_agent = VASP::parent_address(sender);
        let (ret, index) = find_order(order_id);
        
        assert(ret == true, 10002);
        
        let order_list = borrow_global_mut<OrderList>(ADMIN_ACCOUNT_ADDRESS);
        let order = Vector::borrow<Order>(&order_list.orders, index);        

        let balance = DiemAccount::balance<Token>(sender);
        assert(balance > order.price, 100003);

        let incentive_amount = FixedPoint32::multiply_u64(order.price, *&order.sale_incentive);
        
        let sender_withdraw_cap = DiemAccount::extract_withdraw_capability(sig);
        
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

        if(exists<OrderList>(sender)) {
            let sender_order_list = borrow_global_mut<OrderList>(sender);
            Vector::push_back<Order>(&mut sender_order_list.orders, order);
        } else
            move_to<OrderList>(sig, OrderList { orders : Vector::singleton<Order>(order)});

        //emit events
        
    }

    //
    // Find the index of order from the order list
    // order_id : the sha3-256 hash of order
    //
    fun find_order(order_id : &vector<u8>) : (bool, u64) 
    acquires OrderList {
        let order_list = borrow_global<OrderList>(ADMIN_ACCOUNT_ADDRESS);

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
    // public fun fetch_order() : Order {

    // }
}  

}