#ifndef _TESTLIB_H
#define _TESTLIB_H

#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t create_libra_client_proxy(
        const char *host,
        ushort port,
        const char *validator_set_file,
        const char *faucet_account_file,
        bool sync_on_wallet_recovery,
        const char *faucet_server,
        const char *mnemonic_file);

    void destory_libra_client_proxy(uint64_t raw_ptr);

    bool libra_test_validator_connection(uint64_t raw_ptr);

    struct Address
    {
        uint8_t address[32];
        uint64_t index;
    };

    Address libra_create_next_account(uint64_t raw_ptr, bool sync_with_validator);

    struct _Account
    {
        uint8_t address[32];
        uint64_t index;
        uint64_t sequence_number;
        int64_t status;
    };

    struct Accounts
    {
        uint64_t len;
        _Account *data;
    };

    Accounts libra_get_all_accounts(uint64_t raw_ptr);

    void libra_free_all_accounts_buf(Accounts accounts);

    /// Get the latest sequence number from validator for the account specified.
    double libra_get_balance(uint64_t raw_ptr, uint64_t index);

    /// Get the latest sequence number from validator for the account specified.
    uint64_t libra_get_sequence_number(uint64_t raw_ptr, uint64_t index);

    /// Mints coins for the receiver specified.
    void libra_mint_coins(uint64_t raw_ptr, uint64_t index, uint64_t num_coins, bool is_blocking);

    struct _index_sequence
    {
        uint64_t index;
        uint64_t sequence;
    };

    bool libra_transfer_coins_int(
        uint64_t raw_ptr,
        uint64_t sender_account_ref_id,
        uint8_t receiver_addr[], // length 32
        uint64_t num_coins,
        uint64_t gas_unit_price,
        uint max_gas_amount,
        bool is_blocking,
        _index_sequence *result);

#ifdef __cplusplus
}
#endif

#endif