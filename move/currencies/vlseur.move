address 0x1 {

module VLSEUR {
    use 0x1::Association;
    use 0x1::Diem;
    use 0x1::FixedPoint32;

    struct T { }

    public fun initialize(account: &signer): (Diem::MintCapability<T>, Diem::BurnCapability<T>) {
        Association::assert_is_association(account);
        // Register the EUR currency.
        Diem::register_currency<T>(
            account,
            FixedPoint32::create_from_rational(1, 2), // exchange rate to LBR
            false,   // is_synthetic
            1000000, // scaling_factor = 10^6
            100,     // fractional_part = 10^2
            x"455552", // UTF8 encoding of "USD" in hex
        )
    }
}

}
