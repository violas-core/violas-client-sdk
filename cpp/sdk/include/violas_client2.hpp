#pragma once

#include <string_view>
#include <memory>
#include <tuple>
#include <coroutine>

#include <diem_types.hpp>
#include <utils.hpp>
#include <bcs_serde.hpp>
#include <json_rpc.hpp>

#include "wallet.hpp"

namespace dt = diem_types;
using ta = dt::TransactionArgument;

namespace violas
{
    inline static const dt::AccountAddress STD_LIB_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};          // 0x1
    inline static const dt::AccountAddress VIOLAS_LIB_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}};       // 0x2
    inline static const dt::AccountAddress ROOT_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0A, 0x55, 0x0C, 0x18}}; // 0xA550C18
    inline static const dt::AccountAddress TC_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0B, 0x1E, 0x55, 0xED}};   // 0xB1E55ED
    inline static const dt::AccountAddress TESTNET_DD_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xDD};

    inline static const size_t ACCOUNT_ROOT_ID = std::numeric_limits<size_t>::max();
    inline static const size_t ACCOUNT_TC_ID = std::numeric_limits<size_t>::max() - 1;
    inline static const size_t ACCOUNT_DD_ID = std::numeric_limits<size_t>::max() - 2;

    inline static const uint64_t MICRO_COIN = 1'000'000;

    using Address = std::array<uint8_t, 16>;
    using AuthenticationKey = std::array<uint8_t, 32>;

    struct EventBase
    {
        uint64_t sequence_number;
        uint64_t transaction_version;
    };

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
        virtual std::tuple<size_t, dt::AccountAddress>
        create_next_account(std::optional<dt::AccountAddress> opt_address = std::nullopt) = 0;

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
        virtual std::tuple<dt::AccountAddress, uint64_t>
        execute_script_bytecode(size_t account_index,
                                std::vector<uint8_t> script,
                                std::vector<dt::TypeTag> type_tags,
                                std::vector<dt::TransactionArgument> args,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 100) = 0;

        virtual std::tuple<dt::AccountAddress, uint64_t>
        execute_script_file(size_t account_index,
                            std::string_view script_file_name,
                            std::vector<dt::TypeTag> type_tags,
                            std::vector<dt::TransactionArgument> args,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100) = 0;

        virtual Task<std::tuple<dt::AccountAddress, uint64_t>>
        await_execute_script(size_t account_index,
                                  std::string_view script_file_name,
                                  std::vector<dt::TypeTag> &&type_tags,
                                  std::vector<dt::TransactionArgument> &&args,
                                  uint64_t max_gas_amount = 1'000'000,
                                  uint64_t gas_unit_price = 0,
                                  std::string_view gas_currency_code = "VLS",
                                  uint64_t expiration_timestamp_secs = 100) = 0;        

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
        virtual dt::SignedTransaction
        sign_multi_agent_script(size_t account_index,
                                dt::Script &&script,
                                std::vector<dt::AccountAddress> secondary_signer_addresses,
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
         * @return std::tuple<dt::AccountAddress, uint64_t>
         *          return the sender's address and sequence number
         */
        virtual std::tuple<dt::AccountAddress, uint64_t>
        sign_and_submit_multi_agent_signed_txn(size_t account_index,
                                               dt::SignedTransaction &&txn) = 0;
        /**
         * @brief Check the VM status of transaction, if the VM status is not "executed" it throw a exception with error info
         *
         * @param address           account address
         * @param sequence_number   the sequence number, both of them indicated a transaction sent by an account
         * @param error_info        if vm status is not "executed", throw an exception with error_info
         */
        virtual void
        check_txn_vm_status(const dt::AccountAddress &address,
                            uint64_t sequence_number,
                            std::string_view error_info) = 0;

        virtual Task<void>
        await_check_txn_vm_status(const dt::AccountAddress &address,
                                   uint64_t sequence_number,
                                   std::string_view error_info) = 0;        

        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> &&module_bytes_code) = 0;

        virtual void
        publish_module(size_t account_index,
                       std::string_view module_file_name) = 0;

        virtual std::optional<AccountState2>
        get_account_state(const dt::AccountAddress address) = 0;

        virtual Task<std::optional<AccountState2>>
        await_get_account_state(const dt::AccountAddress address) = 0;

        virtual std::vector<json_rpc::EventView>
        get_events(EventHandle handle, uint64_t start, uint64_t limit) = 0;

        template <typename T>
        std::vector<T> query_events(EventHandle handle, uint64_t start, uint64_t limit)
        {
            std::vector<T> events;

            for (auto &e : this->get_events(handle, start, limit))
            {
                auto unknow_event = std::get<json_rpc::UnknownEvent>(e.event);

                BcsSerde serde(std::get<json_rpc::UnknownEvent>(e.event).bytes);
                T event;

                serde &&event;

                event.sequence_number = e.sequence_number;
                event.transaction_version = e.transaction_version;

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
        create_parent_vasp_account(const dt::AccountAddress &address,
                                   const std::array<uint8_t, 32> &auth_key,
                                   std::string_view human_name,
                                   bool add_all_currencies = false) = 0;

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const dt::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  std::string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) = 0;

        virtual void
        create_designated_dealer_ex(std::string_view currency_code,
                                    uint64_t sliding_nonce,
                                    const dt::AccountAddress &address,
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
            dt::AccountAddress dd_address) = 0;

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
             dt::AccountAddress dd_address,
             uint64_t tier_index) = 0;
    };

    using client2_ptr = std::shared_ptr<Client2>;

    inline dt::TypeTag
    make_struct_type_tag(dt::AccountAddress address,
                         std::string_view module,
                         std::string_view name)
    {
        return dt::TypeTag{
            dt::TypeTag::Struct{
                address,
                dt::Identifier{std::string{module}},
                dt::Identifier{std::string{name}}}};
    }

    inline dt::StructTag
    make_struct_tag(dt::AccountAddress address,
                    std::string_view module,
                    std::string_view name,
                    std::vector<dt::TypeTag> type_tags)
    {
        return dt::StructTag{
            address,
            dt::Identifier{std::string{module}},
            dt::Identifier{std::string{name}},
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
            else if constexpr (is_same<decay_t<decltype(arg)>, string_view>::value ||
                               is_same<decay_t<decltype(arg)>, string>::value)
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

template <>
template <typename Serializer>
void serde::Serializable<violas::ResourcePath>::serialize(const violas::ResourcePath &obj, Serializer &serializer)
{
    serializer.increase_container_depth();
    serde::Serializable<decltype(obj.path)>::serialize(obj.path, serializer);
    serializer.decrease_container_depth();
}