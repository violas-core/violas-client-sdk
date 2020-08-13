script  {

use 0x1::Oracle;
use 0x1::FixedPoint32;

fun main<CoinType1, CoinType2>(        
        amount_coin1 : u64, 
        amount_coin2: u64
    ){    
    
    let (ex_rate1, _timestamp) = Oracle::get_exchange_rate<CoinType1>();
    let amount_usd = FixedPoint32::multiply_u64(amount_coin1, ex_rate1);
    

    let (ex_rate2, _timestamp) = Oracle::get_exchange_rate<CoinType2>();
    let ex_coin2 = FixedPoint32::divide_u64(amount_usd, ex_rate2);

    assert(amount_coin2 == ex_coin2, 1002);    
}

}