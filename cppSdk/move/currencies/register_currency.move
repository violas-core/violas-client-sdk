script {
use 0x1::FixedPoint32;
use 0x1::Association;
use 0x1::Libra;
use 0x1::LibraAccount;

fun main<NewCurrency>(
    association: &signer,
    exchange_rate_denom: u64,
    exchange_rate_num: u64,
    is_synthetic: bool,
    scaling_factor: u64,
    fractional_part: u64,
    currency_code: vector<u8>,
)  {
    Association::assert_is_association(association);

    // exchange rate to LBR
    let rate = FixedPoint32::create_from_rational(
        exchange_rate_denom,
        exchange_rate_num,
    );

    let (mint_cap, burn_cap) = Libra::register_currency<NewCurrency>(
        association,
        rate,
        is_synthetic, 
        scaling_factor,
        fractional_part,
        currency_code,
    );

    LibraAccount::add_currency_capability_to_treasury_compliance_account(association, mint_cap, burn_cap);

    //Libra::publish_mint_capability(association,  mint_cap);
    
    //Libra::publish_burn_capability(association, burn_cap);

    //LibraAccount::add_currency_by_address<NewCurrency>(association,0xFEE);
    
}

}