#pragma once
#include <string_view>
#include <memory>
#include <tuple>
#include <diem_types.hpp>
#include <utils.hpp>
#include <bcs_serde.hpp>
#include <json_rpc.hpp>
#include "wallet.hpp"

#if defined(__GNUC__) && !defined(__llvm__)
#include <coroutine>
#endif

namespace dt = diem_types;
using ta = diem_types::TransactionArgument;

namespace violas
{
    inline static const diem_types::AccountAddress STD_LIB_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};          // 0x1
    inline static const diem_types::AccountAddress VIOLAS_LIB_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}};       // 0x2
    inline static const diem_types::AccountAddress ROOT_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0A, 0x55, 0x0C, 0x18}}; // 0xA550C18
    inline static const diem_types::AccountAddress TC_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0B, 0x1E, 0x55, 0xED}};   // 0xB1E55ED
    inline static const diem_types::AccountAddress TESTNET_DD_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xDD};

    inline static const size_t ACCOUNT_ROOT_ID = std::numeric_limits<size_t>::max();
    inline static const size_t ACCOUNT_TC_ID = std::numeric_limits<size_t>::max() - 1;
    inline static const size_t ACCOUNT_DD_ID = std::numeric_limits<size_t>::max() - 2;

    inline static const uint64_t MICRO_COIN = 1'000'000;

    using Address = std::array<uint8_t, 16>;
    using AuthenticationKey = std::array<uint8_t, 32>;

    struct EventHandle
    {
        uint64_t counter;
        std::vector<uint8_t> guid;

        BcsSerde &serde(BcsSerde &serde)
        {
            return serde && counter && guid;
        }
    };

    struct ResourcePath
    {
        std::variant<dt::ModuleId, dt::StructTag> path;

        ResourcePath(const dt::StructTag &tag)
        {
            path = tag;
        }

        ResourcePath(const dt::ModuleId &module_id)
        {
            path = module_id;
        }

        inline std::vector<uint8_t> bcsSerialize() const
        {
            auto serializer = serde::BcsSerializer();
            serde::Serializable<ResourcePath>::serialize(*this, serializer);
            return std::move(serializer).bytes();
        }
    };

    class AccountState2
    {
        std::map<std::vector<uint8_t>, std::vector<uint8_t>> _resources;

    public:
        AccountState2(const std::string &hex);

        template <typename T>
        std::optional<T> get_resource(dt::StructTag tag)
        {
            ResourcePath path{tag};

            auto iter = _resources.find(path.bcsSerialize());
            if (iter != end(_resources))
            {
                T t;

                BcsSerde serde(iter->second);

                serde &&t;

                return t;
            }
            else
                return {};
        }
    };

    class Client2 : public std::enable_shared_from_this<Client2>
    {
    public:
        static std::shared_ptr<Client2>
        create(std::string_view url,
               uint8_t chain_id,
               std::string_view mnemonic,
               std::string_view mint_key);

        virtual ~Client2(){};

        // Create the next account by inner wallet
        // return index and address
        virtual std::tuple<size_t, diem_types::AccountAddress>
        create_next_account(std::optional<diem_types::AccountAddress> opt_address = std::nullopt) = 0;

        virtual void
        update_account_info(size_t account_index) = 0;

        virtual std::vector<Wallet::Account>
        get_all_accounts() = 0;
        /**
         * @brief Submit a transaction  with script bytes and return the sequence number of account index
         *
         * @param account_index
         * @param script
         * @param type_tags
         * @param args
         * @param max_gas_amount
         * @param gas_unit_price
         * @param gas_currency_code
         * @param expiration_timestamp_secs
         * @return uint64_t
         */
        virtual std::tuple<diem_types::AccountAddress, uint64_t>
        execute_script_bytecode(size_t account_index,
                                std::vector<uint8_t> script,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 100) = 0;

        virtual std::tuple<diem_types::AccountAddress, uint64_t>
        execute_script_file(size_t account_index,
                            std::string_view script_file_name,
                            std::vector<diem_types::TypeTag> type_tags,
                            std::vector<diem_types::TransactionArgument> args,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100) = 0;

#if defined(__GNUC__) && !defined(__llvm__)
        virtual void
        async_submit_script(size_t account_index,
                            std::string_view script_file_name,
                            std::vector<diem_types::TypeTag> type_tags,
                            std::vector<diem_types::TransactionArgument> args,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100,
                            std::function<void(diem_types::AccountAddress, uint64_t)> callback = nullptr) = 0;

        auto await_execute_script(size_t account_index,
                                  std::string_view script_file_name,
                                  std::vector<diem_types::TypeTag> &&type_tags,
                                  std::vector<diem_types::TransactionArgument> &&args,
                                  uint64_t max_gas_amount = 1'000'000,
                                  uint64_t gas_unit_price = 0,
                                  std::string_view gas_currency_code = "VLS",
                                  uint64_t expiration_timestamp_secs = 100)
        {
            struct awaitable
            {
                std::shared_ptr<Client2> _client;
                size_t account_index;
                std::string_view script_file_name;
                std::vector<diem_types::TypeTag> type_tags;
                std::vector<diem_types::TransactionArgument> args;
                uint64_t max_gas_amount = 1'000'000;
                uint64_t gas_unit_price = 0;
                std::string_view gas_currency_code = "VLS";
                uint64_t expiration_timestamp_secs = 100;

                diem_types::AccountAddress _sender_address;
                uint64_t _sequence_number;

                bool await_ready() { return false; }
                auto await_resume() { return std::make_tuple<>(_sender_address, _sequence_number); }
                void await_suspend(std::coroutine_handle<> h)
                {
                    _client->async_submit_script(account_index, script_file_name, type_tags, args,
                                                 max_gas_amount, gas_unit_price, gas_currency_code, expiration_timestamp_secs,
                                                 [h, this](diem_types::AccountAddress address, uint64_t sn) mutable
                                                 {
                                                     this->_sender_address = address;
                                                     this->_sequence_number = sn;
                                                     h.resume();
                                                 });
                }
            };

            return awaitable{shared_from_this(), account_index, script_file_name, std::move(type_tags), std::move(args)};
        }
#endif
        /**
         * @brief Sign a multi agent script bytes code and return a signed txn which contains sender authenticator and no secondary signature
         *
         * @param account_index
         * @param script
         * @param type_tags
         * @param args
         * @param secondary_signer_addresses
         * @param max_gas_amount
         * @param gas_unit_price
         * @param gas_currency_code
         * @param expiration_timestamp_secs
         * @return SignedTransaction
         */
        virtual diem_types::SignedTransaction
        sign_multi_agent_script(size_t account_index,
                                diem_types::Script &&script,
                                std::vector<diem_types::AccountAddress> secondary_signer_addresses,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 100) = 0;
        /**
         * @brief Submit a multi agent signed transaction
         *
         * @param account_index
         * @param txn
         * @param secondary_signer_addresse
         * @return std::tuple<diem_types::AccountAddress, uint64_t>
         *          return the sender's address and sequence number
         */
        virtual std::tuple<diem_types::AccountAddress, uint64_t>
        sign_and_submit_multi_agent_signed_txn(size_t account_index,
                                               diem_types::SignedTransaction &&txn) = 0;
        /**
         * @brief Check the VM status of transaction, if the VM status is not "executed" it throw a exception with error info
         *
         * @param address           account address
         * @param sequence_number   the sequence number, both of them indicated a transaction sent by an account
         * @param error_info        if vm status is not "executed", throw an exception with error_info
         */
        virtual void
        check_txn_vm_status(const diem_types::AccountAddress &address,
                            uint64_t sequence_number,
                            std::string_view error_info) = 0;

        virtual void
        async_check_txn_vm_status(const diem_types::AccountAddress &address,
                                  uint64_t sequence_number,
                                  std::function<void()> callback) = 0;

#if defined(__GNUC__) && !defined(__llvm__)
        auto await_check_txn_vm_status(const diem_types::AccountAddress &address,
                                       uint64_t sequence_number,
                                       std::string_view error_info)
        {
            struct awaitable
            {
                const diem_types::AccountAddress &address;
                uint64_t sequence_number;

                bool await_ready() { return false; }
                void await_suspend(std::coroutine_handle<> h)
                {
                    // std::jthread([h]
                    //              { h.resume(); });
                }
                void await_resume() {}
            };

            return awaitable{address, sequence_number};
        }
#endif
        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> &&module_bytes_code) = 0;

        virtual void
        publish_module(size_t account_index,
                       std::string_view module_file_name) = 0;

        virtual std::optional<AccountState2>
        get_account_state(const dt::AccountAddress address) = 0;

        virtual std::vector<json_rpc::EventView>
        get_events(EventHandle handle, uint64_t start, uint64_t limit) = 0;

        template <typename T>
        std::vector<T> query_events(EventHandle handle, uint64_t start, uint64_t limit)
        {
            std::vector<T> events;

            for (auto &e : this->get_events(handle, start, limit))
            {
                T event;
                BcsSerde serde(std::get<json_rpc::UnknownEvent>(e.event).bytes);

                serde &&event;

                events.push_back(event);
            }

            return events;
        }
        ////////////////////////////////////////////////////////////////
        // Methods for Violas framework
        ////////////////////////////////////////////////////////////////
        virtual void
        add_currency(size_t account_index, std::string_view currency_code) = 0;

        virtual void
        allow_custom_script(bool is_allowing) = 0;

        virtual void
        allow_publishing_module(bool is_allowing) = 0;

        virtual uint64_t
        create_parent_vasp_account(const diem_types::AccountAddress &address,
                                   const std::array<uint8_t, 32> &auth_key,
                                   std::string_view human_name,
                                   bool add_all_currencies = false) = 0;

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const diem_types::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  std::string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) = 0;

        virtual void
        create_designated_dealer_ex(std::string_view currency_code,
                                    uint64_t sliding_nonce,
                                    const diem_types::AccountAddress &address,
                                    const std::array<uint8_t, 32> &auth_key,
                                    std::string_view human_name,
                                    bool add_all_currencies) = 0;
        /**
         * @brief Registers a stable currency coin
         *
         * @param currency_code
         * @param exchange_rate_denom
         * @param exchange_rate_num
         * @param scaling_factor
         * @param fractional_part
         */
        virtual void
        regiester_stable_currency(std::string_view currency_code,
                                  uint64_t exchange_rate_denom,
                                  uint64_t exchange_rate_num,
                                  uint64_t scaling_factor,
                                  uint64_t fractional_part) = 0;

        /**
         * @brief Add a currency for DD account, this mehtod needs TC account Permission
         *
         * @param dd_address DD account address
         */
        virtual void
        add_currency_for_designated_dealer(
            std::string_view currency_code,
            diem_types::AccountAddress dd_address) = 0;

        /**
         * @brief mint amount of currency to a DD account
         *
         * @param currency_code
         * @param amount
         * @param dd_address
         */
        virtual void
        mint(std::string_view currency_code,
             uint64_t sliding_nonce,
             uint64_t amount,
             diem_types::AccountAddress dd_address,
             uint64_t tier_index) = 0;
    };

    using client2_ptr = std::shared_ptr<Client2>;

    inline diem_types::TypeTag
    make_struct_type_tag(diem_types::AccountAddress address,
                         std::string_view module,
                         std::string_view name)
    {
        return diem_types::TypeTag{
            diem_types::TypeTag::Struct{
                address,
                diem_types::Identifier{std::string{module}},
                diem_types::Identifier{std::string{name}}}};
    }

    inline dt::StructTag
    make_struct_tag(diem_types::AccountAddress address,
                    std::string_view module,
                    std::string_view name,
                    std::vector<dt::TypeTag> type_tags)
    {
        return dt::StructTag{
            address,
            diem_types::Identifier{std::string{module}},
            diem_types::Identifier{std::string{name}},
            type_tags,
        };
    }

    template <typename... Args>
    std::vector<ta> make_txn_args(Args &&...args)
    {
        using namespace std;
        vector<ta> txn_args;

        auto to_ta = [&](auto &&arg)
        {
            if constexpr (is_same<decay_t<decltype(arg)>, uint8_t>::value)
            {
                txn_args.push_back({ta::U8{arg}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, uint64_t>::value)
            {
                txn_args.push_back({ta::U64{arg}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, __uint128_t>::value)
            {
                txn_args.push_back({ta::U128{serde::uint128_t{uint64_t(arg >> 64), uint64_t(arg)}}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, dt::AccountAddress>::value)
            {
                txn_args.push_back({ta::Address{arg}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, bytes>::value)
            {
                txn_args.push_back({ta::U8Vector{arg}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, string_view>::value)
            {
                txn_args.push_back({ta::U8Vector{bytes(begin(arg), end(arg))}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, array<uint8_t, 32>>::value)
            {
                txn_args.push_back({ta::U8Vector{bytes(begin(arg), end(arg))}});
            }
            else if constexpr (is_same<decay_t<decltype(arg)>, bool>::value)
            {
                txn_args.push_back({ta::Bool{arg}});
            }
            else
                // can't use 'false' -- expression has to depend on a template parameter
                static_assert(!sizeof(arg), "The argument type of transaction is unsupported.");
        };

        // fold expression in C++ 17
        ((to_ta(args)), ...);

        return txn_args;
    }

    void run_test_case(std::string_view url,
                       uint8_t chain_id,
                       std::string_view mint_key,
                       std::string_view mnemonic);
}