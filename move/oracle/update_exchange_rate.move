script  {

use 0x1::Oracle;

fun main<CoinType>(
        lr_account : &signer, 
        numerator : u64, 
        denominator: u64
    ){
    
    Oracle::update_exchange_rate<CoinType>(lr_account, numerator, denominator);
}

}