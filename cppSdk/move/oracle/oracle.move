address 0x1 {

module Oracle {

use 0x1::LibraTimestamp;
use 0x1::Signer;
use 0x1::CoreAddresses;
use 0x1::FixedPoint32::{Self, FixedPoint32};
//use 0x1::LibraAccount;

    ///   Exchange rate, EUR / USD
    resource struct ExchangeRate<Currency1, Currency2> { 
        value : FixedPoint32,
        timestamp : u64,    //Unix time in microseconds
    }

    const EINVALID_SINGLETON_ADDRESS: u64 = 0;

    // public fun initialize(lr_account: &signer) {
    //     assert(
    //         Signer::address_of(lr_account) == CoreAddresses::LIBRA_ROOT_ADDRESS(),
    //         EINVALID_SINGLETON_ADDRESS
    //     );
    // }

    public fun initialize_exchange_rate<Currency1, Currency2>(lr_account : &signer, numerator : u64, denominator: u64)
    {
        assert(
            Signer::address_of(lr_account) == CoreAddresses::LIBRA_ROOT_ADDRESS(),
            EINVALID_SINGLETON_ADDRESS
        );        

        move_to(
            lr_account, 
            ExchangeRate<Currency1, Currency2> {
                value : FixedPoint32::create_from_rational(numerator, denominator), 
                timestamp : LibraTimestamp::now_microseconds(),
            });
    }

    /// update exchange rate for the currency pair
    public fun update_exchange_rate<Currency1, Currency2>(lr_account : &signer, numerator : u64, denominator: u64) acquires ExchangeRate {
        assert(
            Signer::address_of(lr_account) == CoreAddresses::LIBRA_ROOT_ADDRESS(),
            EINVALID_SINGLETON_ADDRESS
        );

        let exchange_rate = borrow_global_mut<ExchangeRate<Currency1, Currency2>>(CoreAddresses::LIBRA_ROOT_ADDRESS());

        exchange_rate.value = FixedPoint32::create_from_rational(numerator, denominator);
        exchange_rate.timestamp = LibraTimestamp::now_microseconds();
    }

    public fun get_exchange_rate<Currency1, Currency2>() : (FixedPoint32, u64) acquires ExchangeRate    
    {
        let exchange_rate = borrow_global<ExchangeRate<Currency1, Currency2>>(CoreAddresses::LIBRA_ROOT_ADDRESS());

        (*&exchange_rate.value, exchange_rate.timestamp)
    }
}


}