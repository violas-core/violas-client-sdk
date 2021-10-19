address 0x2 {

module Sale {
    use Std::Signer;
    use Std::Vector;

    struct Order has store {
        price : u64,
    } 

    struct OrderList has key, store {
        orders : vector<Order>,
    }

    const ADMIN_ACCOUNT_ADDRESS : address = @1122;

    //
    //
    //
    public fun check_admin_permission(sig : &signer) {
        assert(Signer::address_of(sig) == ADMIN_ACCOUNT_ADDRESS, 1000);
    }

    //
    //
    //
    public fun initialize(sig: &signer) {
        
        check_admin_permission(sig);

        move_to(sig, OrderList {
            orders : Vector::empty<Order>()
        });
    }
    //
    //
    //
    public fun make_order(price : u64) 
    acquires OrderList {
        let order_list = borrow_global_mut<OrderList>(ADMIN_ACCOUNT_ADDRESS);

        Vector::push_back<Order>(&mut order_list.orders, Order { price });
    }

    //
    //
    //
    public fun take_order(_sale_agnet : &signer, _customer : &signer, _order_id : vector<u8>) {

    }

    //
    //
    //
    public find(order_id : vecotr<u8>) : (bool, u64) {
        let order_list = borrow_global<OrderList>(ADMIN_ACCOUNT_ADDRESS);

        let i = 0;
        let len = length(v);
        while (i < len) {
            let order = Vector::borrow(& order_list.orders, i);
                
            if ( == e) 
                return (true, i);
            
            i = i + 1;
        };

        (false, 0)
    }
    // public fun fetch_order() : Order {

    // }
}  

}