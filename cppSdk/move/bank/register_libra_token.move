script {
use 0x1::ViolasBank;

fun main<Token>(account: &signer, price_oracle: address, collateral_factor: u64, tokendata: vector<u8>) {
    ViolasBank::register_libra_token<Token>(account, price_oracle, collateral_factor, tokendata);
}
}
