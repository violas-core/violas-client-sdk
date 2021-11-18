#include <diem_framework.hpp>
#include "../include/violas_client2.hpp"

#include "../include/json_rpc.hpp"
#include "wallet.hpp"

using namespace std;

namespace violas
{
    class Client2Imp : public Client2
    {
    private:
        json_rpc::client_ptr m_jr_client;
        
        Wallet m_wallet;

    public:
        Client2Imp(std::string_view url,
                   uint8_t chain_id,
                   std::string_view mint_key,
                   std::string_view mnemonic)
        {
            m_jr_client = json_rpc::Client::create(url);
        }

        ~Client2Imp()
        {
        }

        virtual tuple<size_t,diem_types::AccountAddress>
        create_next_account() override
        {
            m_wallet.create_next_account();

            return make_tuple<>(0, diem_types::AccountAddress());
        }

        virtual std::vector<diem_types::AccountAddress>
        get_all_accounts() override
        {
            m_wallet.get_all_accounts();
            return std::vector<diem_types::AccountAddress>();
        }

        void submit_script(size_t account_index,
                           diem_types::Script &&script,
                           uint64_t max_gas_amount = 1'000'000,
                           uint64_t gas_unit_price = 0,
                           std::string_view gas_currency_code = "VLS",
                           uint64_t expiration_timestamp_secs = 600)
        {
            using namespace diem_types;
            SignedTransaction signed_txn;
            RawTransaction &raw_txn = signed_txn.raw_txn;

            raw_txn.payload.value = TransactionPayload::Script({std::move(script)});
            raw_txn.sender = AccountAddress({m_wallet.get_account_address(account_index)});
            raw_txn.sequence_number = 0;
            raw_txn.max_gas_amount = max_gas_amount;
            raw_txn.gas_unit_price = gas_unit_price;
            raw_txn.gas_currency_code = gas_currency_code;
            raw_txn.expiration_timestamp_secs = expiration_timestamp_secs;

            m_jr_client->submit(signed_txn);
        }

        virtual void
        submit_script_byte_code(size_t account_index,
                                std::vector<uint8_t> script_byte_code,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
                                uint64_t max_gas_amount,
                                uint64_t gas_unit_price,
                                std::string_view gas_currency_code,
                                uint64_t expiration_timestamp_secs) override
        {
            this->submit_script(account_index,
                                diem_types::Script({script_byte_code, type_tags, args}),
                                max_gas_amount,
                                gas_unit_price,
                                gas_currency_code,
                                expiration_timestamp_secs);
        }

        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> module) override
        {
        }

        virtual void
        add_currency(size_t account_index, std::string_view currency_code) override
        {
            submit_script(account_index,
                          diem_framework::encode_add_currency_to_account_script(diem_types::TypeTag({})));
        }
    };

    std::shared_ptr<Client2>
    Client2::create(std::string_view url,
                    uint8_t chain_id,
                    std::string_view mnemonic,
                    std::string_view mint_key)
    {
        return make_shared<Client2Imp>(url, chain_id, mnemonic, mint_key);
    }

}