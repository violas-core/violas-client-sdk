address 0x1 {

module VLS {
    use 0x1::AccountLimits;
    use 0x1::CoreAddresses;
    use 0x1::Errors;
    use 0x1::FixedPoint32;
    use 0x1::Libra::{Self, Libra};
    //use 0x1::LibraAccount;
    use 0x1::LibraTimestamp;
    //use 0x1::Signer;
    
    /// The type tag representing the `VLS` currency on-chain.
    struct VLS { }

    /// VLS holds mint capability for mining
    resource struct Reserve {
        /// The mint capability allowing minting of `VLS` coins.
        mint_cap: Libra::MintCapability<VLS>,
        /// The burn capability for `VLS` coins. This is used for the unpacking
        /// of `VLS` coins into the underlying backing currencies.
        burn_cap: Libra::BurnCapability<VLS>,
        /// The preburn for `VLS`. This is an administrative field since we
        /// need to alway preburn before we burn.
        preburn_cap: Libra::Preburn<VLS>,
    }

    resource struct Receivers {
        miner : address,
    }

    /// The `Reserve` resource is in an invalid state
    const ERESERVE: u64 = 0;
    const EZERO_VLS_MINT_NOT_ALLOWED: u64 = 3;
    const VLS_TOTAL_AMOUNT:u64 = 100000000;   //10^8

    /// Initializes the `VLS` module. This creates the mint, preburn, and burn
    /// capabilities for `VLS` coins
    public fun initialize(
        lr_account: &signer,
        tc_account: &signer,
    ) {
        //LibraTimestamp::assert_genesis();

        // Operational constraint
        CoreAddresses::assert_currency_info(lr_account);
        // Reserve must not exist.
        assert(!exists<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS()), Errors::already_published(ERESERVE));
        let (mint_cap, burn_cap) = Libra::register_currency<VLS>(
            lr_account,
            FixedPoint32::create_from_rational(1, 1), // exchange rate to VLS
            false,    // is_synthetic
            1000000, // scaling_factor = 10^6
            1000,    // fractional_part = 10^3
            b"VLS"
        );

        AccountLimits::publish_unrestricted_limits<VLS>(lr_account);
        let preburn_cap = Libra::create_preburn<VLS>(tc_account);
        
        move_to(lr_account, Reserve { mint_cap, burn_cap, preburn_cap });
    }

    /// Returns true if `CoinType` is `VLS::VLS`
    public fun is_vls<CoinType>(): bool {
        Libra::is_currency<CoinType>() &&
            Libra::currency_code<CoinType>() == Libra::currency_code<VLS>()
    }

    spec fun is_vls {
        pragma verify = false, opaque = true;
        /// The following is correct because currency codes are unique.
        ensures result == spec_is_VLS<CoinType>();
    }

    /// Returns true if CoinType is VLS.
    spec define spec_is_vls<CoinType>(): bool {
        type<CoinType>() == type<VLS>()
    }

    /// * If `amount_VLS` is zero the function will abort.
    fun mint(
        amount_vls: u64,
    ): Libra<VLS>
    acquires Reserve {              

        assert(amount_vls > 0, Errors::invalid_argument(EZERO_VLS_MINT_NOT_ALLOWED));
        
        let reserve = borrow_global_mut<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS());              
                
        // Once the coins have been deposited in the reserve, we can mint the VLS
        Libra::mint_with_capability<VLS>(amount_vls, &reserve.mint_cap)
    }

    spec fun create {
        pragma opaque;
        modifies global<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS());
        modifies global<Libra::CurrencyInfo<VLS>>(CoreAddresses::CURRENCY_INFO_ADDRESS());
        include CreateAbortsIf;
        let reserve = global<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS());
                
        ensures exists<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS());
        include Libra::MintEnsures<VLS>{value: amount_VLS};
    }

    spec schema CreateAbortsIf {
        amount_vls: u64;
        
        let reserve = global<Reserve>(CoreAddresses::LIBRA_ROOT_ADDRESS());
        aborts_if amount_vls == 0 with Errors::INVALID_ARGUMENT;
        
        include LibraTimestamp::AbortsIfNotOperating;
        
        include Libra::MintAbortsIf<VLS>{value: amount_VLS};
        include CalculateComponentAmountsForVLSAbortsIf;
    }

    /// Distribute VLS to all the specified account 
    public fun distribute() 
    acquires Receivers, Reserve {
        LibraTimestamp::assert_operating();
        let _time_seconds = LibraTimestamp::now_seconds();

        let vls_coin = mint(100);

        let miner = *&borrow_global<Receivers>(CoreAddresses::LIBRA_ROOT_ADDRESS()).miner;

        //LibraAccount::deposit<VLS>(CoreAddresses::VM_RESERVED_ADDRESS(), receivers.miner, vls_coin, x"", x"")
         // Deposit the `to_deposit` coin
        //Libra::deposit(LibraAccount::balance<VLS>(miner), vls_coin);
        //move_to(miner, LibraAccount::Balance<VLS>{ coin: Libra::zero<VLS>() });

    }
}

}
