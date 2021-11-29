#pragma once
#include <string_view>
#include <memory>
#include <tuple>
#include <diem_types.hpp>
#include "wallet.hpp"

namespace violas
{
    class Client2
    {
    public:
        inline static const diem_types::AccountAddress STD_LIB_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};          // 0x1
        inline static const diem_types::AccountAddress ROOT_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0A, 0x55, 0x0C, 0x18}}; // 0xA550C18
        inline static const diem_types::AccountAddress TC_ADDRESS{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0B, 0x1E, 0x55, 0xED}};   // 0xB1E55ED
        inline static const diem_types::AccountAddress TESTNET_DD_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xDD};

        const size_t ACCOUNT_ROOT_ID = std::numeric_limits<size_t>::max();
        const size_t ACCOUNT_TC_ID = std::numeric_limits<size_t>::max() - 1;
        const size_t ACCOUNT_DD_ID = std::numeric_limits<size_t>::max() - 2;

        static std::shared_ptr<Client2>
        create(std::string_view url,
               uint8_t chain_id,
               std::string_view mnemonic,
               std::string_view mint_key);

        virtual ~Client2(){};

        // Create the next account by inner wallet
        // return index and address
        virtual std::tuple<size_t, diem_types::AccountAddress>
        create_next_account() = 0;

        virtual std::vector<Wallet::Account>
        get_all_accounts() = 0;

        virtual void
        submit_script_byte_code(size_t account_index,                                
                                std::vector<uint8_t> script,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 100) = 0;

        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> module) = 0;

        virtual void
        add_currency(size_t account_index, std::string_view currency_code) = 0;

        virtual void
        create_parent_vasp_account(const diem_types::AccountAddress &address,
                                   const std::array<uint8_t, 32> &auth_key,
                                   std::string_view human_name,
                                   bool add_all_currencies) = 0;
    };

    using client2_ptr = std::shared_ptr<Client2>;

    void run_test_case(std::string_view url,
                       uint8_t chain_id,
                       std::string_view mint_key,
                       std::string_view mnemonic);
}