script {
    use 0x1::Exchange;

    fun add_reserve<Token1, Token2>(admin: &signer) {
        Exchange::add_reserve<Token1, Token2>(admin);
    }
}