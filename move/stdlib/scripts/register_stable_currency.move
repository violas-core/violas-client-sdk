script {
use DiemFramework::Diem;
use DiemFramework::AccountLimits;
use Std::FixedPoint32;
///
/// Registers a stable currency (SCS) coin
///
fun register_stable_currency<CoinType: store>(
    dr_account: signer,
    tc_account: signer,
    exchange_rate_denom: u64,
    exchange_rate_num: u64,    
    scaling_factor: u64,
    fractional_part: u64,
    currency_code: vector<u8>,
)  {
    // exchange rate to LBR
    let rate = FixedPoint32::create_from_rational(
        exchange_rate_denom,
        exchange_rate_num,
    ); 

   Diem::register_SCS_currency<CoinType>(
            &dr_account,
            &tc_account,
            rate,
            scaling_factor,
            fractional_part,
            currency_code, 
            );
    
    AccountLimits::publish_unrestricted_limits<CoinType>(&dr_account);
}

}