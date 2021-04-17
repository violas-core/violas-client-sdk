address 0x1 {

module Exchange {

use 0x1::Signer;
use 0x1::Errors;
//use 0x1::FixedPoint32;
use 0x1::Diem; //::{Self};
use 0x1::Vector;
use 0x1::DiemAccount::{Self};
use 0x1::Event::{ Self, EventHandle };
    
    struct AddLiquidityEvent {      
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,        
    }

    struct RemoveLiquidityEvent {                
         currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,        
    }

    struct DepositeLiquidityEvent {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct WithdrawLiquidityEvent {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct SwapEvent {        
        
        currency1_code: vector<u8>,
        
        currency1_amount: u64,
        
        currency2_code: vector<u8>,
        
        currency2_amount: u64,
    }

    struct CurrencyPairCode {
        currency1_code : vector<u8>,
        currency2_code : vector<u8>
    }

    resource struct ReserveInfo {
        currency_pair_codes : vector<CurrencyPairCode>
    }
    ///
    /// Fund pool held under administrator account 
    //
    resource struct Reserve<Token1, Token2>{
        token1_amount : u64,
        token2_amount : u64,
        add_liquidity_events: EventHandle<AddLiquidityEvent>,
        //remove_liquidity_events: EventHandle<RemoveLiquidityEvent>,
    }
    ///
    /// Liquidity held under sender account
    ///
    resource struct Liquidity<Token1, Token2>{
        amount : u64,        
        deposite_liquidity_events: EventHandle<DepositeLiquidityEvent>,
        withdraw_liquidity_events: EventHandle<WithdrawLiquidityEvent>,
        swap_events: EventHandle<SwapEvent>,        
    }
    
    resource struct Capability {
        withdraw_cap: DiemAccount::WithdrawCapability,
    }
    //
    //  Error code
    //
    const E_INVALID_ADMIN_ADDRESS: u64 = 1000;
    const E_RESERVE_HAS_NOT_BEEN_PUBLISHED : u64 = 1001;
    const E_RESERVE_HAS_PUBLISHED : u64 = 1002;
    const E_LIQUIDITY_IS_NOT_PUBLISHED : u64 = 1003;
    const EDO_NOT_HAVE_ENOUGH_LIQUIDITY : u64 = 1004;
    const E_DO_NOT_HAVE_ENOUGH_RESERVE : u64 = 1005;
    const E_DO_NOT_MEET_TOKEN2_OUTPUT_AMOUNT : u64 = 1006;
    const E_CALCULATION_ERROR : u64 = 1007;
    const E_TOKEN2_AMOUNT_IS_NOT_ENOUGH : u64 = 1008;
    
    ///
    /// Public functions
    ///

    /// initialize Exchange by admin account
    public fun initialize(admin : &signer) {
        assert(
            Signer::address_of(admin) == admin_address(), 
            E_INVALID_ADMIN_ADDRESS
        ); 

        // Hold administrator's pay_to_sender capability for depositing and withdrawing global reserve liquidity
        move_to(admin, Capability {
            withdraw_cap: DiemAccount::extract_withdraw_capability(admin)
        });

        move_to(admin, ReserveInfo {
            currency_pair_codes : Vector::empty()
        });
    }
    
    // native fun create_signer(addr: address): signer;
    // native fun destroy_signer(sig: signer);

    /// add reserve info by admin account
    public fun add_reserve<Token1, Token2>(admin : &signer) 
    acquires ReserveInfo {
        assert(
            Signer::address_of(admin) == admin_address(), 
            Errors::requires_address(E_INVALID_ADMIN_ADDRESS)
        ); 
        
        DiemAccount::accepts_currency<Token1>(admin_address());
        DiemAccount::accepts_currency<Token2>(admin_address());

        assert( !exists<Reserve<Token1, Token2>>(admin_address()) && 
                !exists<Reserve<Token2, Token1>>(admin_address()),
                Errors::already_published(E_RESERVE_HAS_PUBLISHED));
        
        let reserve = Reserve<Token1, Token2> {
                token1_amount : 0,
                token2_amount : 0,
                add_liquidity_events : Event::new_event_handle<AddLiquidityEvent>(admin)
            };

        move_to(admin, reserve);  
        // let admin_signer = create_signer(admin_address());
        // move_to(&admin_signer, reserve);  
        // destroy_signer(admin_signer);

        // add reserver info
        let reserve_info = borrow_global_mut<ReserveInfo>(admin_address());
        Vector::push_back(&mut reserve_info.currency_pair_codes, CurrencyPairCode {
            currency1_code : Diem::currency_code<Token1>(),
            currency2_code : Diem::currency_code<Token2>(),
        });        
    }
    ///
    /// deposit liquidity by user account
    ///
    public fun deposit_liquidity<Token1, Token2>(
        sender : &signer,
        token1_amount : u64,
        token2_amount : u64
    ) acquires Reserve, Liquidity {      
        let sender_address = Signer::address_of(sender);

        // 1. add liquidity to reserver
        (token1_amount, token2_amount) = deposit_reserve_liquidity<Token1, Token2>(token1_amount, token2_amount);

        // 2. pay token1 and token2 to admin
        pay_to_admin<Token1>(sender, token1_amount);
        pay_to_admin<Token2>(sender, token2_amount); 

        // 3. update liquidity for sender account
        if( !exists<Liquidity<Token1, Token2>>(sender_address) ) {
            let reserve = Liquidity<Token1, Token2> {
                amount : sqrt(token1_amount, token2_amount),
                deposite_liquidity_events : Event::new_event_handle<DepositeLiquidityEvent>(sender),
                withdraw_liquidity_events : Event::new_event_handle<WithdrawLiquidityEvent>(sender),
                swap_events : Event::new_event_handle<SwapEvent>(sender),
            };

            move_to(sender, reserve);            
        } else {
            // Update local liquidity
            let liquidity = borrow_global_mut<Liquidity<Token1, Token2>>(sender_address); 

            liquidity.amount  = liquidity.amount + sqrt(token1_amount, token2_amount)            
        };              
    }

    public fun withdraw_liquidity<Token1, Token2>(
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

    public fun swap<Token1, Token2>(
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
    fun check_tokens_foward<Token1, Token2>() : bool {
        if (exists<Reserve<Token1, Token2>>(admin_address())) {            
            true
        } else if (exists<Reserve<Token2, Token1>>(admin_address())) {            
            false
        } else {
            abort(Errors::not_published(E_RESERVE_HAS_NOT_BEEN_PUBLISHED))
        }
    }

    fun pay_to_admin<Token>(account: &signer, amount: u64) {
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

    fun pay_to_sender<Token>(payee: address, amount: u64) acquires Capability {
        let cap = borrow_global<Capability>(admin_address());

        DiemAccount::pay_from<Token>(
            &cap.withdraw_cap,
            payee,
            amount,
            x"",
            x""
        )
    }

    fun deposit_reserve_liquidity<Token1, Token2>(token1_amount : u64, token2_amount : u64) : (u64, u64)
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

    
    fun withdraw_reserve_liquidity<Token1, Token2>(liquidity_amount : u64) : (u64, u64)
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