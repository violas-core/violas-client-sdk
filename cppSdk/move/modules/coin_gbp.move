address 0x1 {    //0xA550C18

module VLSGBP {
    use 0x1::Association;
    use 0x1::Libra;
    use 0x1::FixedPoint32;

    struct T { }

    public fun initialize(account: &signer): (Libra::MintCapability<T>, Libra::BurnCapability<T>) {
        Association::assert_is_association(account);
        // Register the GBP currency.
        Libra::register_currency<T>(
            account,
            FixedPoint32::create_from_rational(1, 2), // exchange rate to LBR
            false,   // is_synthetic
            1000000, // scaling_factor = 10^6
            100,     // fractional_part = 10^2
            x"474250", // UTF8 encoding of "GBP" in hex
        )
    }
}

}
