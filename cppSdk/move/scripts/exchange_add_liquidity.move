script {
use 0xA550C18::Exchange;
fun main<Token>(min_liquidity: u64, max_token_amount: u64, violas_amount: u64, deadline: u64) {
    Exchange::add_liquidity<Token>(min_liquidity, max_token_amount, violas_amount, deadline);
}
}