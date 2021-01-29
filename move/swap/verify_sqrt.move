script {
    use 0x1::Exchange;

    fun verify_sqrt() {
        Exchange::verify_sqrt();
    }
}