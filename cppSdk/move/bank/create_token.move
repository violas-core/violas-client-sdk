script {
use 0x1::ViolasBank;

fun main(account: &signer, currency_code: vector<u8>, owner: address, price_oracle: address, collateral_factor: u64, tokendata: vector<u8>) {
    ViolasBank::create_token(account, currency_code, owner, price_oracle, collateral_factor, tokendata);
}
}
