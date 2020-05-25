script {
use 0xA550C18::Exchange;
fun main<Token>(amount: u64, min_violas: u64, min_tokens: u64, deadline: u64) {
    Exchange::remove_liquidity<Token>(amount, min_violas, min_tokens, deadline);
}
}