script {
use DiemFramework::DiemAccount;

fun main<NewCurrency: store>(
    dr_root: signer,
    exchange_rate_denom: u64,
    exchange_rate_num: u64,
    is_synthetic: bool,
    scaling_factor: u64,
    fractional_part: u64,
    currency_code: vector<u8>,
)  {

    DiemAccount::register_currency_by_root_account<NewCurrency>(
        &dr_root, 
        exchange_rate_denom, 
        exchange_rate_num,
        is_synthetic,
        scaling_factor,
        fractional_part,
        currency_code);    
    
}

}