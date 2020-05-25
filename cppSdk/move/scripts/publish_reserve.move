script {
use 0xA550C18::Exchange;
fun main<Token>() {
    Exchange::publish_reserve<Token>();
}

}