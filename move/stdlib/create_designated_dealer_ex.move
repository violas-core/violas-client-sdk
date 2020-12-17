script {
use 0x1::DiemAccount;
use 0x1::SlidingNonce;

fun create_designated_dealer_ex<CoinType>(
        tc_account: &signer,
        sliding_nonce: u64,
        new_account_address: address,
        auth_key: vector<u8>,
        human_name: vector<u8>,
        add_all_currencies: bool,
    )
{
    SlidingNonce::record_nonce_or_abort(tc_account, sliding_nonce);
    DiemAccount::create_designated_dealer_ex<CoinType>(
        tc_account, 
        new_account_address, 
        auth_key,
        human_name,
        add_all_currencies);
}

spec fun create_designated_dealer_ex {
    use 0x1::Errors;
    use 0x1::Roles;

    include DiemAccount::TransactionChecks{sender: tc_account}; // properties checked by the prologue.
    include SlidingNonce::RecordNonceAbortsIf{account: tc_account, seq_nonce: sliding_nonce};
    include DiemAccount::CreateDesignatedDealerAbortsIf<Currency>{
        creator_account: tc_account, new_account_address: addr};
    include DiemAccount::CreateDesignatedDealerEnsures<Currency>{new_account_address: addr};

    aborts_with [check]
        Errors::INVALID_ARGUMENT,
        Errors::REQUIRES_ADDRESS,
        Errors::NOT_PUBLISHED,
        Errors::ALREADY_PUBLISHED,
        Errors::REQUIRES_ROLE;

    /// **Access Control:**
    /// Only the Treasury Compliance account can create Designated Dealer accounts [[A5]][ROLE].
    include Roles::AbortsIfNotTreasuryCompliance{account: tc_account};
}

}