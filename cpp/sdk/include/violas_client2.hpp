#pragma once
#include <string_view>
#include <memory>
#include <tuple>
#include <diem_types.hpp>

namespace violas
{
    class Client2
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
        virtual std::tuple<size_t,diem_types::AccountAddress>
        create_next_account() = 0;

        virtual std::vector<diem_types::AccountAddress>
        get_all_accounts() = 0;

        virtual void
        submit_script_byte_code(size_t account_index,
                                std::vector<uint8_t> script,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 900) = 0;

        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> module) = 0;

        virtual void
        add_currency(size_t account_index, std::string_view currency_code) = 0;
    };

    using client2_ptr = std::shared_ptr<Client2>;

    void run_test_case(std::string_view url,
                       uint8_t chain_id,
                       std::string_view mint_key,
                       std::string_view mnemonic);
}