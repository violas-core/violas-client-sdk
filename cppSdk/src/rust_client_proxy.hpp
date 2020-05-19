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
    //  crete a Violas client proxy pointer
    //
    bool violas_create_client(const char *url,
                              const char *mint_key_file_name,
                              bool sync_on_wallet_recovery,
                              const char *faucet_server,
                              const char *mnemonic_file_name,
                              const char *waypoint,
                              uint64_t *out_raw_ptr);

    void libra_destory_client_proxy(uint64_t raw_ptr);

    bool libra_test_validator_connection(uint64_t raw_ptr);

    const size_t ADDRESS_LENGTH = 16;

    struct Libra_Address
    {
        uint8_t address[ADDRESS_LENGTH];
        uint64_t index;
    };

    Libra_Address libra_create_next_account(uint64_t raw_ptr, bool sync_with_validator);

    struct _Account
    {
        uint8_t address[ADDRESS_LENGTH];
        uint8_t auth_key[32]; //authentication key
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
    bool libra_get_balance(uint64_t raw_ptr, const uint8_t address[ADDRESS_LENGTH], uint64_t *result);

    /// Get the latest sequence number from validator for the account specified.
    bool libra_get_sequence_number(uint64_t raw_ptr, const uint8_t address[ADDRESS_LENGTH], uint64_t &sequence_num);

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

    bool libra_compile(uint64_t raw_ptr, const char *account_index_or_addr, const char *script_file_path, bool is_module,
                       const char *temp_dir);

    bool libra_publish_module(uint64_t raw_ptr, uint64_t account_index, const char *module_file);

    /// association transaction with local faucet account
    bool violas_publish_module_with_association_account(uint64_t raw_ptr, const char *module_file);

    struct ScriptArgs
    {
        uint64_t len;
        const char **data; // C string array
    };

    bool libra_execute_script(uint64_t raw_ptr, uint64_t account_index, const char *script_file, const ScriptArgs *script_args);

    bool violas_execute_script_with_association_account(uint64_t raw_ptr, const char *script_file, const ScriptArgs *script_args);

    //
    //  get committed transaction and events info
    //  note that :  you must call 'libra_free_string' for argumets 'out_txn' and 'events' after calling 'libra_get_committed_txn_by_acc_seq'
    //
    bool libra_get_committed_txn_by_acc_seq(uint64_t raw_ptr, const uint8_t address[], uint64_t sequence_num, char **out_txn, char **events);

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

    enum libra_event_type
    {
        sent = true,
        received = false
    };
    struct StrArrray
    {
        const char **data;
        uint64_t len;
        uint64_t cap;
    };
    //
    //  Get events by account and event type with start sequence number and limit.
    //  address : account address
    //  type : event type, true for sent, false for false
    //  start_seq_number : start sequence number
    //  limit : limit for fetched amount
    bool libra_get_events(uint64_t raw_ptr,
                          const uint8_t address[],
                          libra_event_type type,
                          uint64_t start_seq_number,
                          uint64_t limit,
                          StrArrray *out_all_txn_events,
                          char **out_last_event_state);

    //
    //
    //
    void violas_free_str_array(StrArrray *str_array);

    //
    bool libra_get_account_resource(uint64_t raw_ptr,
                                    const char *account_index_or_addr,
                                    const char *account_path_addr,
                                    uint64_t token_index,
                                    uint64_t *balance);
    ///
    /// the following all functions for multi currencies
    ///
    struct ViolasTypeTag
    {
        uint8_t address[ADDRESS_LENGTH];
        const char *module;
        const char *name;
    };

    // add a new currency
    bool violas_add_currency(uint64_t raw_client,
                             const ViolasTypeTag &violas_type_tag,
                             uint64_t exchange_rate_denom,
                             uint64_t exchange_rate_num,
                             bool is_synthetic,
                             uint64_t scaling_factor,
                             uint64_t fractional_part,
                             const char *currency_code,
                             uint64_t currency_code_len);

    /// register a currency
    bool violas_register_currency(uint64_t raw_client,
                                  const ViolasTypeTag &violas_type_tag,
                                  uint64_t account_index,
                                  bool is_blocking);

    /// register a currency with association account
    bool violas_register_currency_with_association_account(uint64_t raw_client,
                                                           const ViolasTypeTag &violas_type_tag,
                                                           bool is_blocking);

    /// mint coins for a receiver for a speciafied currency
    bool violas_mint_currency(uint64_t raw_client,
                              const ViolasTypeTag &violas_type_tag,
                              const uint8_t receiver_auth_key[32],
                              uint64_t amount,
                              bool is_blocking);

    /// transfer currency from payer to payee
    bool violas_transfer_currency(uint64_t raw_client,
                                  const ViolasTypeTag &violas_type_tag,
                                  uint64_t sender_account_index,
                                  uint8_t receiver_auth_key[32],
                                  uint64_t amount,
                                  bool is_blocking);

#ifdef __cplusplus
}
#endif

#endif