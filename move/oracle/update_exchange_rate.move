script  {

use 0x1::Oracle;

fun main<CoinType: store>(
        dr_account : signer, 
        numerator : u64, 
        denominator: u64
    ){
    
    Oracle::update_exchange_rate<CoinType>(&dr_account, numerator, denominator);
}

}