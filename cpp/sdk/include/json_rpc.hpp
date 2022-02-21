#ifndef JSON_RPC
#define JSON_RPC
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <variant>
#include <functional>
#include <diem_types.hpp>
#include <bcs_serde.hpp>

#if defined(__GNUC__) && !defined(__llvm__)
#include <coroutine>
#endif

namespace json_rpc
{
    namespace dt = diem_types;

    struct Currency
    {
        std::string code;
    };

    struct Balance
    {
        uint64_t amount;
        std::string currency;
    };

    struct AccountView
    {
        diem_types::AccountAddress address;
        std::vector<Balance> balances;
        uint64_t sequence_number;
        diem_types::EventKey sent_events_key;
        diem_types::EventKey received_events_key;
        bool delegated_key_rotation_capability;
        bool delegated_withdrawal_capability;
        bool is_frozen;
    };

    struct AccountStateProof
    {
        // hex-encoded bcs bytes
        std::string ledger_info_to_transaction_info_proof; //[json_name = "ledger_info_to_transaction_info_proof"];
        // hex-encoded bcs bytes
        std::string transaction_info; //[json_name = "transaction_info"];
        // hex-encoded bcs bytes
        std::string transaction_info_to_account_proof; //[json_name = "transaction_info_to_account_proof"];
    };

    struct AccountStateWithProof
    {
        uint64_t version;
        // hex-encoded bcs bytes
        std::string blob;
        // hex-encoded bcs bytes
        AccountStateProof proof;
    };

    struct UnknownEvent
    {
        std::vector<uint8_t> bytes;
    };

    struct EventView
    {
        std::string key;
        uint64_t sequence_number;
        uint64_t transaction_version;
        std::variant<UnknownEvent> event;
    };

    struct Script
    {
    };

    struct TransactionData
    {
        std::string type; // Type of TransactionData

        std::string sender;           // Hex-encoded account address of the sender
        std::string signature_scheme; // Signature scheme used by the sender to sign this transaction
        std::string signature;        // Hex-encoded signature of this transaction signed by the sender
        std::string public_key;       // Hex-encoded public key of the transaction sender

        std::string secondary_signers;           // Hex-encoded account addresses of the secondary signers
        std::string secondary_signature_schemes; // Signature schemes used by the secondary signers to sign this transaction
        std::string secondary_signatures;        // Hex-encoded signatures of this transaction signed by the primary signers
        std::string secondary_public_keys;       // Hex-encoded public keys of the secondary signers

        uint64_t sequence_number; // Sequence number of this transaction corresponding to sender's account
        uint8_t chain_id;         // Chain ID of the Diem network this transaction is intended for
        uint64_t max_gas_amount;  // Maximum amount of gas that can be spent for this transaction
        uint64_t gas_unit_price;
        std::string gas_currency;           // Gas price currency code
        uint64_t expiration_timestamp_secs; // The expiration time (Unix Epoch in seconds) for this transaction

        std::string script_hash;  // Hex-encoded sha3 256 hash of the script binary code bytes used in this transaction
        std::string script_bytes; // Hex-encoded string of BCS bytes of the script, decode it to get back transaction script arguments
        Script script;            // The transaction script and arguments of this transaction,
                                  // you can decode script_bytes by BCS to get same data.
    };

    struct VMStatus
    {
        struct Executed
        {
            std::string type;
        };

        struct ExecutionFailure
        {
            std::string type;
            std::string location;
            uint64_t function_index;
            uint64_t code_offset;
        };

        struct OutOfGas
        {
            std::string type;
        };

        struct MiscellaneousError
        {
            std::string type;
        };

        struct MoveAbort
        {
            std::string type;
            std::string location;
            uint64_t abort_code;
            struct Explanation
            {
                std::string category;
                std::string category_description;
                std::string reason;
                std::string reason_description;
            } explanation;
        };

        std::variant<Executed, ExecutionFailure, OutOfGas, MiscellaneousError, MoveAbort> value;
    };

    struct TransactionView
    {
        uint64_t version;            // The on-chain version or unique identifier of this transaction
        TransactionData txn_data;    // Transaction payload
        std::string hash;            // hex encoded string of hash of this transaction, hex encoded
        std::string bytes;           // hex encoded string of raw bytes of the transaction, hex encoded
        std::list<EventView> events; // List of associated events. Empty for no events
        VMStatus vm_status;          // The returned status of the transaction after being processed by the VM
        uint64_t gas_used;           // Amount of gas used by this transaction, to know how much you paid for the transaction,
                                     // you need multiply it with your RawTransaction#gas_unit_price
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////

    struct Client : public std::enable_shared_from_this<Client>
    {
        static std::shared_ptr<Client>
        create(std::string_view url);

        virtual ~Client() {}
        /**
         * @brief Submit a singed transaction to validator or full node
         *
         * @param signed_txn a signed transaction
         */
        virtual void submit(const diem_types::SignedTransaction &signed_txn) = 0;

#if defined(__GNUC__) && !defined(__llvm__)
        //
        //  Async submit
        //
        virtual void async_submit(const diem_types::SignedTransaction &signed_txn,
                                  std::function<void()> callback) = 0;
        //
        // co_wait await_submit
        //
        auto await_submit(diem_types::SignedTransaction &&signed_txn)
        {
            struct awaitable
            {
                std::shared_ptr<Client> _client;
                diem_types::SignedTransaction signed_txn;

                bool await_ready() { return false; }
                void await_resume() {}
                void await_suspend(std::coroutine_handle<> h)
                {
                    _client->async_submit(signed_txn, [h]()
                                          { h.resume(); });
                }
            };

            return awaitable{shared_from_this(), std::move(signed_txn)};
        }
#endif

        virtual std::optional<TransactionView>
        get_account_transaction(const diem_types::AccountAddress &address,
                                uint64_t sequence_number,
                                bool include_events) = 0;

        virtual void
        async_get_account_transaction(const diem_types::AccountAddress &address,
                                      uint64_t sequence_number,
                                      bool include_events,
                                      std::function<void(TransactionView &)>) = 0;

#if defined(__GNUC__) && !defined(__llvm__)
        auto await_get_account_transaction(const diem_types::AccountAddress &address,
                                           uint64_t sequence_number,
                                           bool include_events)
        {
            struct awaitable
            {
                std::shared_ptr<Client> _client;
                diem_types::AccountAddress address;
                uint64_t sequence_number;
                bool include_events;

                TransactionView _txn_view;

                bool await_ready() { return false; }
                auto await_resume() { return _txn_view; }
                void await_suspend(std::coroutine_handle<> h)
                {
                    _client->async_get_account_transaction(
                        address,
                        sequence_number,
                        include_events,
                        [h, this](TransactionView &view)
                        { 
                            _txn_view = view;
                            h.resume(); });
                }
            };

            return awaitable{
                shared_from_this(),
                address,
                sequence_number,
                include_events};
        }
#endif
        virtual std::optional<AccountView>
        get_account(const diem_types::AccountAddress &, std::optional<uint64_t> version = std::nullopt) = 0;

        virtual std::vector<Currency>
        get_currencies() = 0;

        virtual AccountStateWithProof
        get_account_state_blob(std::string account_address) = 0;

        virtual std::vector<EventView>
        get_events(std::string event_key, uint64_t start, uint64_t limit, uint64_t rpc_id = 1) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
}

#endif