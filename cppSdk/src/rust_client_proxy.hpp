#ifndef _TESTLIB_H
#define _TESTLIB_H

#ifdef __cplusplus
extern "C"
{
#endif
    //
    // when the function return false, call get_last_error to get the detail
    //
    char *libra_get_last_error();
    //
    // free the the memory of string got from Rust
    //
    void libra_free_string(char *str);
    //
    //  crete a Libra client proxy pointer
    //
    uint64_t libra_create_client_proxy(
        const char *host,
        uint64_t port,
        const char *validator_set_file,
        const char *faucet_account_file,
        bool sync_on_wallet_recovery,
        const char *faucet_server,
        const char *mnemonic_file);

    void libra_destory_client_proxy(uint64_t raw_ptr);

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
    bool libra_get_balance(uint64_t raw_ptr, const char* account_index_or_addr, double *result);

    /// Get the latest sequence number from validator for the account specified.
    uint64_t libra_get_sequence_number(uint64_t raw_ptr, uint64_t index);

    /// Mints coins for the receiver specified.
    bool libra_mint_coins(uint64_t raw_ptr, uint64_t index, uint64_t num_coins, bool is_blocking);

    struct _index_sequence
    {
        uint64_t index;
        uint64_t sequence;
    };

    bool libra_transfer_coins_int(
        uint64_t raw_ptr,
        uint64_t sender_account_ref_id,
        uint8_t receiver_addr[], // length 32
        uint64_t micro_coins,    // 1 / 1000000
        uint64_t gas_unit_price,
        uint64_t max_gas_amount,
        bool is_blocking,
        _index_sequence *result);

    bool libra_compile(uint64_t raw_ptr, const char *account_index_or_addr, const char *script_file_path, bool is_module);

    bool libra_publish_module(uint64_t raw_ptr, uint64_t account_index, const char *module_file);

    struct ScriptArgs
    {
        uint64_t len;
        const char **data; // C string array
    };

    bool libra_execute_script(uint64_t raw_ptr, uint64_t account_index, const char *script_file, const ScriptArgs *script_args);

    //
    //  get committed transaction and events info
    //  note that :  you must call 'libra_free_string' for argumets 'out_txn' and 'events' after calling 'libra_get_committed_txn_by_acc_seq'
    //
    bool libra_get_committed_txn_by_acc_seq(uint64_t raw_ptr, uint64_t account_index, uint64_t sequence_num, char **out_txn, char **events);

    struct TxnEvents
    {
        const char *transaction;
        const char *events;
    };

    struct AllTxnEvents
    {
        const TxnEvents *data;
        uint64_t len;
        uint64_t cap;
    };

    //
    //  Get transactions in range (start_version..start_version + limit - 1) from validator.
    //  raw_pt : raw pointor created by libra_create_client_proxy
    //  start_version : start version
    //  limit : limit ammount
    //  fetch_events : whether fetch events
    //
    bool libra_get_txn_by_range(uint64_t raw_ptr, uint64_t start_version, uint64_t limit, bool fetch_events, AllTxnEvents *all_txn_events);

    void libra_free_all_txn_events(AllTxnEvents *all_txn_events);

    bool libra_get_account_resource(uint64_t raw_ptr, const char *account_index_or_addr, const char *account_path_addr, uint64_t *balance);

#ifdef __cplusplus
}
#endif

#endif