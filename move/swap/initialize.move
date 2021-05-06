script {
    use 0x1::Exchange;

    fun initialize(admin: signer) {
        Exchange::initialize(&admin);
    }
}