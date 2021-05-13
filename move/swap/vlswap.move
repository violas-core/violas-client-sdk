address 0x1 {


module Exchange {

use 0x1::Signer;
use 0x1::Errors;
use 0x1::FixedPoint32::{FixedPoint32};    //Self
//use 0x1::Diem; //::{Self};
use 0x1::Vector;
use 0x1::DiemAccount::{Self};
use 0x1::Event::{ Self, EventHandle };
// Fees for initialization
use 0x1::Fee500::Fee500;
use 0x1::Fee3000::Fee3000;
use 0x1::Fee10000::Fee10000;
    
    struct AddLiquidityEvent has drop, store {      
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,        
    }

    struct RemoveLiquidityEvent has drop, store {                
         currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,        
    }

    struct DepositeLiquidityEvent has drop, store {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct WithdrawLiquidityEvent has drop, store {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct SwapEvent has drop, store {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct CurrencyPairCode has store {
        currency1_code : vector<u8>,
        currency2_code : vector<u8>
    }

    struct ReserveInfo has key, store {
        currency_pair_codes : vector<CurrencyPairCode>
    }
    ///
    /// Fund pool held under administrator account 
    //
    struct Reserve<Token1, Token2> has key, store {
        token1_amount : u64,
        token2_amount : u64,
        add_liquidity_events: EventHandle<AddLiquidityEvent>,
        remove_liquidity_events: EventHandle<RemoveLiquidityEvent>,
    }
    ///
    /// Liquidity held under sender account
    ///
    struct Liquidity<Token1, Token2> has key, store {
        amount : u64,        
        deposite_liquidity_events: EventHandle<DepositeLiquidityEvent>,
        withdraw_liquidity_events: EventHandle<WithdrawLiquidityEvent>,
        swap_events: EventHandle<SwapEvent>,        
    }
    
    struct Capability has key, store {
        withdraw_cap: DiemAccount::WithdrawCapability,
    }

    struct Fee<FeeType> has key, store {
        fee : u64,
        tick_spacing : u64,
        fee_code : vector<u8>
    }
    
    struct RegisteredFees has key, store {
        fee_codes : vector<vector<u8>>
    }

    ///
    /// Reseve Pool held under admin account
    ///
    struct Pool<Currency1, Currency2, FeeType> has key, store {        
        sqrt_price : FixedPoint32,
        tick : u64,

        c1_amount : u64,
        c2_amount : u64,
        
        add_liquidity_events: EventHandle<AddLiquidityEvent>,
        //remove_liquidity_events: EventHandle<RemoveLiquidityEvent>,
    }

    struct PoolInfo has store {
        currency1_code : vector<u8>,
        currency2_code : vector<u8>,
        fee_code : vector<u8>
    }
    
    ///
    /// RegisteredPools for enuming all pool info in client
    ///
    struct RegisteredPools has key, store {
        pool_codes : vector<PoolInfo>,   
    }

    ///
    /// Liquidity held under provider account
    ///
    struct LiquidityT<Currency1, Currency2, FeeType> has key, store {
        tick_lower : u64,
        tick_upper : u64,
        c1_amount : u64,
        c2_amount : u64,        
        add_liquidity_events: EventHandle<AddLiquidityEvent>,
        remove_liquidity_events: EventHandle<RemoveLiquidityEvent>,               
    }

    //
    //  Error code
    //
    const E_NOT_ADMIN_ACCOUNT: u64 = 1000;
    const E_RESERVE_HAS_NOT_BEEN_PUBLISHED : u64 = 1001;
    const E_POOL_HAS_ALREADY_PUBLISHED : u64 = 1002;
    const E_LIQUIDITY_IS_NOT_PUBLISHED : u64 = 1003;
    const EDO_NOT_HAVE_ENOUGH_LIQUIDITY : u64 = 1004;
    const E_DO_NOT_HAVE_ENOUGH_RESERVE : u64 = 1005;
    const E_DO_NOT_MEET_TOKEN2_OUTPUT_AMOUNT : u64 = 1006;
    const E_CALCULATION_ERROR : u64 = 1007;
    const E_TOKEN2_AMOUNT_IS_NOT_ENOUGH : u64 = 1008;
    const E_FEE_TYPE : u64 = 1010;
    const E_LIQUIDITY : u64 = 1020;
    
    ///
    /// Public functions
    ///

    /// initialize Exchange by admin account
    public fun initialize(admin : &signer) 
    acquires RegisteredFees {
        
        assert(
            Signer::address_of(admin) == admin_address(), 
            E_NOT_ADMIN_ACCOUNT
        ); 

        // Hold administrator's pay_to_sender capability for depositing and withdrawing global reserve liquidity
        move_to(admin, Capability {
            withdraw_cap: DiemAccount::extract_withdraw_capability(admin)
        });

        move_to(admin, ReserveInfo {
            currency_pair_codes : Vector::empty()
        });

        move_to(admin, RegisteredFees {
            fee_codes :  Vector::empty()
        });

        // Register all initial fees with tick spacing
        register_fee_tick_spacing<Fee500>(admin, 500, 10, b"Fee500");
        register_fee_tick_spacing<Fee3000>(admin, 3000, 60, b"Fee3000");
        register_fee_tick_spacing<Fee10000>(admin, 10000, 200, b"Fee100000");
    }
    
    ///
    /// register a new fee rate with tick spacing
    ///
    public fun register_fee_tick_spacing<FeeType: store>(admin : &signer, fee : u64, tick_spacing : u64, fee_code : vector<u8>) 
    acquires RegisteredFees {
        
        let sender =  Signer::address_of(admin);
        assert(sender == admin_address(), Errors::requires_role(E_NOT_ADMIN_ACCOUNT));
        
        assert(!exists<Fee<FeeType>>(sender), Errors::already_published(E_POOL_HAS_ALREADY_PUBLISHED));

        move_to(admin, Fee<FeeType> {
            fee,
            tick_spacing,
            fee_code : copy fee_code,
        });

        let registered_fees = borrow_global_mut<RegisteredFees>(sender);

        Vector::push_back(&mut registered_fees.fee_codes, fee_code); 
    }

    ///
    /// Register a new Pool by adminitrator
    /// Price is represented as a sqrt(amountToken1/amountToken0)
    ///
    public fun register_pool<Currency1: store, Currency2: store, FeeType: store>(
        admin : &signer, 
        sqrt_price : FixedPoint32
    ) {        
        // check admin permission
        assert( Signer::address_of(admin) == admin_address(), 
                Errors::requires_role(E_NOT_ADMIN_ACCOUNT));

        // check if the pool has already been published .
        assert( !exists<Pool<Currency1, Currency2, FeeType>>(admin_address()) && 
                !exists<Pool<Currency2, Currency1, FeeType>>(admin_address()),
                Errors::already_published(E_POOL_HAS_ALREADY_PUBLISHED));

        // check if the admin account accepts currency 1
        assert( DiemAccount::accepts_currency<Currency1>(admin_address()), 
                Errors::not_published(E_NOT_ADMIN_ACCOUNT));
        
        // check if the admin account accepts currency 2
        assert( DiemAccount::accepts_currency<Currency2>(admin_address()), 
                Errors::not_published(E_NOT_ADMIN_ACCOUNT));
        
        //  Check if FeeType has been published
        assert( exists<Fee<FeeType>>(admin_address()), 
                Errors::not_published(E_FEE_TYPE));
        
        move_to(admin, Pool<Currency1, Currency2, FeeType> {
            sqrt_price,
            tick : 0,            
            c1_amount : 0,
            c2_amount : 0,
            add_liquidity_events : Event::new_event_handle<AddLiquidityEvent>(admin),
        });
    }

    ///
    /// Add liqudity by Provider
    ///
    public fun add_liquidity<Currency1: store, Currency2: store, FeeType: store>(
        sender : &signer, 
        tick_lower : u64,
        tick_upper : u64,
        c1_amount : u64,
        c2_amount : u64
    ) acquires Pool {
        let sender_address = Signer::address_of(sender);

        assert(exists<LiquidityT<Currency1, Currency2, FeeType>>(sender_address), Errors::already_published(E_LIQUIDITY));

        let (required_c1_amount, required_c2_amount) = add_pool_liquidity<Currency1, Currency2, FeeType>(tick_lower, tick_upper, c1_amount, c2_amount);

        move_to(sender, LiquidityT<Currency1, Currency2, FeeType> {
            tick_lower,
            tick_upper,
            c1_amount : required_c1_amount,
            c2_amount : required_c2_amount,
            add_liquidity_events : Event::new_event_handle<AddLiquidityEvent>(sender),
            remove_liquidity_events : Event::new_event_handle<RemoveLiquidityEvent>(sender)
        });
       
       pay_to_admin<Currency1>(sender, c1_amount);
       pay_to_admin<Currency1>(sender, c2_amount);
    }
    ///
    /// deposit liquidity by user account
    ///
    // public fun deposit_liquidity<Token1: store, Token2: store>(
    //     sender : &signer,
    //     token1_amount : u64,
    //     token2_amount : u64
    // ) acquires Reserve, Liquidity {      
    //     let sender_address = Signer::address_of(sender);

    //     // 1. add liquidity to reserver
    //     (token1_amount, token2_amount) = deposit_reserve_liquidity<Token1, Token2>(token1_amount, token2_amount);

    //     // 2. pay token1 and token2 to admin
    //     pay_to_admin<Token1>(sender, token1_amount);
    //     pay_to_admin<Token2>(sender, token2_amount); 

    //     // 3. update liquidity for sender account
    //     if( !exists<Liquidity<Token1, Token2>>(sender_address) ) {
    //         let reserve = Liquidity<Token1, Token2> {
    //             amount : sqrt(token1_amount, token2_amount),
    //             deposite_liquidity_events : Event::new_event_handle<DepositeLiquidityEvent>(sender),
    //             withdraw_liquidity_events : Event::new_event_handle<WithdrawLiquidityEvent>(sender),
    //             swap_events : Event::new_event_handle<SwapEvent>(sender),
    //         };

    //         move_to(sender, reserve);            
    //     } else {
    //         // Update local liquidity
    //         let liquidity = borrow_global_mut<Liquidity<Token1, Token2>>(sender_address); 

    //         liquidity.amount  = liquidity.amount + sqrt(token1_amount, token2_amount)            
    //     };              
    // }

    public fun withdraw_liquidity<Token1: store, Token2: store>(
        sender : &signer,
        amount : u64
    ) acquires Reserve, Liquidity, Capability {        
        let sender_address = Signer::address_of(sender);
        
        assert(exists<Liquidity<Token1, Token2>>(sender_address), Errors::not_published(E_LIQUIDITY_IS_NOT_PUBLISHED));

        // Update liquidity for sender account
        let liquidity = borrow_global_mut<Liquidity<Token1, Token2>>(sender_address); 
        
        assert(liquidity.amount >= amount, EDO_NOT_HAVE_ENOUGH_LIQUIDITY); // Errors::invalid_argument(EDO_NOT_HAVE_ENOUGH_LIQUIDITY)

        liquidity.amount = liquidity.amount - amount;        
        
        // 2. remove reserve liquidity
        let (token1_amount, token2_amount) = withdraw_reserve_liquidity<Token1, Token2>(amount);
        
        // 3. pay token1 and token2 to sender
        pay_to_sender<Token1>(sender_address, token1_amount);
        pay_to_sender<Token2>(sender_address, token2_amount); 

    }

    // public fun get_liquidity_balance<Token1, Token2>( addr : address)
    // acquires Liquidity {
    //     assert(exists<Liquidity<Token1, Token2>>(sender_address), Errors::not_published(E_LIQUIDITY_IS_NOT_PUBLISHED));


    // }

    public fun swap<Token1: store, Token2: store>(
        sender : &signer,
        token1_input_amount : u64,
        token2_output_min_amount : u64
        ) : u64 
        acquires Reserve, Capability {
        
        let is_forward = check_tokens_foward<Token1, Token2>();

        // Get reference for (x, y) of reserve liquidity
        let (x, y) = if ( is_forward ) {            
            let reserve = borrow_global_mut<Reserve<Token1, Token2>>(admin_address());
            (&mut reserve.token1_amount, &mut reserve.token2_amount)
        } else {            
            let reserve = borrow_global_mut<Reserve<Token2, Token1>>(admin_address());
            (&mut reserve.token2_amount, &mut reserve.token1_amount)
        };

        assert(*x !=0 && *y !=0 , E_DO_NOT_HAVE_ENOUGH_RESERVE); //Errors::invalid_argument(E_DO_NOT_HAVE_ENOUGH_RESERVE)

        let dx = token1_input_amount;

        // // a = dx / x;
        // let a = FixedPoint32::create_from_rational(dx, *x);
        // // b = 1 + a;
        // let b = FixedPoint32::create_from_rational(*x + dx, *x);
        // // dy = y * a / (1 + a) = y * a / b, 
        // let dy = FixedPoint32::divide_u64(FixedPoint32::multiply_u64(*y, a), b) ;

        let dy = calculate_exchange_output(*x, *y, dx);

        // dy > min token2 output
        assert(dy >= token2_output_min_amount, E_DO_NOT_MEET_TOKEN2_OUTPUT_AMOUNT);    // Errors::invalid_argument(E_DO_NOT_MEET_TOKEN2_OUTPUT_AMOUNT)
        
        let k = (*x) * (*y);        
        // update reserve liquidity
        *x = *x + dx;
        *y = *y - dy;

        // assert calculation model x' * y' = x * y 
        assert( (*x) * (*y) == k, E_CALCULATION_ERROR);  // Errors::invalid_argument(E_CALCULATION_ERROR)

        // pay_to_admin token1 and pay_to_sender token2 for reserve
        pay_to_admin<Token1>(sender, dx);
        pay_to_sender<Token2>(Signer::address_of(sender), dy);

        // if( is_forward ) {
        //     pay_to_admin<Token1>(sender, dx);
        //     pay_to_sender<Token2>(Signer::address_of(sender), dy);
        // } 
        // else {
        //     pay_to_admin<Token2>(sender, dx);
        //     pay_to_sender<Token1>(Signer::address_of(sender), dy);
        // };

        dy        
    }

    public fun verify_sqrt() {
        let x :u64 = 18446744073709551615; //0xFFFFFFFFFFFFFFFF;
        assert(sqrt(x, x) == x, 3001);

        x = 4294967295; //0xFFFFFFFF;
        assert(sqrt(x, x) == x, 3002);

        x = 245893761;  //
        assert(sqrt(x, x) == x, 3003);

        x = 1;
        assert(sqrt(x, x) == x, 3004);
    }

    ///
    /// Internal functions
    ///
    fun admin_address() : address {
        0xEE
    }

    /// Newton iteration method
    fun sqrt(x : u64, y : u64) : u64
    {
        let z = (x as u128) * (y as u128);
        let z1 = z -1;   
        let s: u8  = 1; // shift bits

        // Initialize s
        if (z1 > ((1 as u128) << 64) - 1) {s = s + 32; z1 = z1 >> 64;};
        if (z1 > ((1 as u128) << 32) - 1) {s = s + 16; z1 = z1 >> 32;};
        if (z1 > 65535) {s = s + 8; z1 = z1 >> 16;};
        if (z1 > 255)   {s = s + 4; z1 = z1 >> 8;};
        if (z1 > 15)    {s = s + 2; z1 = z1 >> 4;};
        if (z1 > 3)     {s = s + 1; };  //z1 = z1 >> 2;

        // iterate
        let g0 : u128 = 1 << s;
        let g1 = (g0 + (z >> s)) >> 1;
        
        while(g1 < g0)
        {
            g0 = g1;
            g1 = (g0 + z / g0) >> 1;
        };

        (g0 as u64)
    }

    fun calculate_exchange_output(x: u64, y: u64, token1_input_amount: u64) : u64 {
        let k = (x as u128) * (y as u128);
        let y1 = k / ( x + token1_input_amount as u128);
        
        y - (y1 as u64)
    }

    /// Check if token1 before token2 in reserve pair
    fun check_tokens_foward<Token1: store, Token2: store>() : bool {
        if (exists<Reserve<Token1, Token2>>(admin_address())) {            
            true
        } else if (exists<Reserve<Token2, Token1>>(admin_address())) {            
            false
        } else {
            abort(Errors::not_published(E_RESERVE_HAS_NOT_BEEN_PUBLISHED))
        }
    }

    fun pay_to_admin<Token: store>(account: &signer, amount: u64) {
        let withdraw_cap = DiemAccount::extract_withdraw_capability(account);

        DiemAccount::pay_from<Token>(
            &withdraw_cap,
            admin_address(),
            amount,
            x"",
            x""
        );

        DiemAccount::restore_withdraw_capability(withdraw_cap);
    }

    fun pay_to_sender<Token: store>(payee: address, amount: u64) acquires Capability {
        let cap = borrow_global<Capability>(admin_address());

        DiemAccount::pay_from<Token>(
            &cap.withdraw_cap,
            payee,
            amount,
            x"",
            x""
        )
    }

    fun deposit_reserve_liquidity<Token1: store, Token2: store>(token1_amount : u64, token2_amount : u64) : (u64, u64)
    acquires Reserve {
        assert(exists<Reserve<Token1, Token2>>(admin_address()), 
            Errors::not_published(E_RESERVE_HAS_NOT_BEEN_PUBLISHED));
              
        // Update reserve liquidity
        let reserve = borrow_global_mut<Reserve<Token1, Token2>>(admin_address()); 
        if( reserve.token1_amount == 0 && reserve.token2_amount == 0) {
            reserve.token1_amount = token1_amount;
            reserve.token2_amount = token2_amount;

            (reserve.token1_amount, reserve.token2_amount)
        } else {
            // let exchange_rate = FixedPoint32::create_from_rational(reserve.token1_amount, reserve.token2_amount);
            // let need_token2_amount = FixedPoint32::divide_u64(token1_amount, exchange_rate);

            let need_token2_amount = (((token1_amount as u128) * (reserve.token2_amount as u128) / (reserve.token1_amount as u128)) as u64);
            
            assert(token2_amount >= need_token2_amount, Errors::invalid_argument(E_TOKEN2_AMOUNT_IS_NOT_ENOUGH));

            reserve.token1_amount = reserve.token1_amount + token1_amount;
            reserve.token2_amount = reserve.token2_amount + need_token2_amount;

            (token1_amount, need_token2_amount)
        }           
    }


    fun add_pool_liquidity<Token1: store, Token2: store, FeeType: store>(
        _tick_lower : u64,
        _tick_upper : u64,
        token1_amount : u64, 
        token2_amount : u64) : (u64, u64)
    acquires Pool {
        assert(exists<Pool<Token1, Token2, FeeType>>(admin_address()), 
            Errors::not_published(E_RESERVE_HAS_NOT_BEEN_PUBLISHED));
              
        // Update reserve liquidity
        let pool = borrow_global_mut<Pool<Token1, Token2, FeeType>>(admin_address()); 
        if( pool.c1_amount == 0 && pool.c2_amount == 0) {
            pool.c1_amount = token1_amount;
            pool.c2_amount = token2_amount;

            (pool.c1_amount, pool.c2_amount)
        } else {
            // let exchange_rate = FixedPoint32::create_from_rational(reserve.token1_amount, reserve.token2_amount);
            // let need_token2_amount = FixedPoint32::divide_u64(token1_amount, exchange_rate);

            let need_token2_amount = (((token1_amount as u128) * (pool.c2_amount as u128) / (pool.c1_amount as u128)) as u64);
            
            assert(token2_amount >= need_token2_amount, Errors::invalid_argument(E_TOKEN2_AMOUNT_IS_NOT_ENOUGH));

            pool.c1_amount = pool.c1_amount + token1_amount;
            pool.c2_amount = pool.c2_amount + need_token2_amount;

            (token1_amount, need_token2_amount)
        }           
    }

    
    fun withdraw_reserve_liquidity<Token1: store, Token2: store>(liquidity_amount : u64) : (u64, u64)
    acquires Reserve {
        assert(exists<Reserve<Token1, Token2>>(admin_address()), 
            Errors::not_published(E_RESERVE_HAS_NOT_BEEN_PUBLISHED));

        // Update reserve liquidity
        let reserve = borrow_global_mut<Reserve<Token1, Token2>>(admin_address());        
        let liquidity_total = sqrt(reserve.token1_amount, reserve.token2_amount);
        
        assert(liquidity_amount <= liquidity_total, Errors::invalid_argument(EDO_NOT_HAVE_ENOUGH_LIQUIDITY));
        
        // let rate1 = FixedPoint32::create_from_rational(liquidity_amount, liquidity_total);
        // let rate2 = FixedPoint32::create_from_rational(liquidity_amount, liquidity_total);

        let token1_amount: u64 = ((reserve.token1_amount as u128) * (liquidity_amount as u128) / (liquidity_total as u128) as u64);
        let token2_amount: u64 = ((reserve.token2_amount as u128) * (liquidity_amount as u128) / (liquidity_total as u128) as u64);

        reserve.token1_amount = reserve.token1_amount - token1_amount;
        reserve.token2_amount = reserve.token2_amount - token2_amount;

        (token1_amount, token2_amount)
    }
}

}