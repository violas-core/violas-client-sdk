script {
use 0x0::FixedPoint32;
use 0x0::Association;
use 0x0::Libra;

fun main<NewCurrency>(
    account: &signer,
    exchange_rate_denom: u64,
    exchange_rate_num: u64,
    is_synthetic: bool,
    scaling_factor: u64,
    fractional_part: u64,
    currency_code: vector<u8>,
)  {
     Association::assert_sender_is_association();

    // exchange rate to LBR
    let rate = FixedPoint32::create_from_rational(
        exchange_rate_denom,
        exchange_rate_num,
    );

    let (mint_cap, burn_cap) = Libra::register_currency<NewCurrency>(
        account,
        rate,
        is_synthetic, 
        scaling_factor,
        fractional_part,
        currency_code,
    );

    Libra::publish_mint_capability(account,  mint_cap);
    
    Libra::publish_burn_capability(account, burn_cap);
}

}