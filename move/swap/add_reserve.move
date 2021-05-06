script {
    use 0x1::Exchange;

    fun add_reserve<Token1: store, Token2: store>(admin: signer) {
        Exchange::add_reserve<Token1, Token2>(&admin);
    }
}