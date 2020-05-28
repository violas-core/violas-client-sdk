script {
use 0xA550C18::Exchange;
fun main<Token>(tokens_sold: u64, min_violas: u64, deadline: u64) {
    Exchange::token_to_violas_swap_input<Token>(tokens_sold, min_violas, deadline);
}
}