script {
use 0xA550C18::Exchange;
fun main<Token>(violas_sold: u64, min_tokens: u64, deadline: u64) {
    Exchange::violas_to_token_swap_input<Token>(violas_sold, min_tokens, deadline);
}
}