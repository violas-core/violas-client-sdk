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
        virtual uint64_t
        submit_script_byte_code(size_t account_index,
                                std::vector<uint8_t> script,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
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
        virtual diem_types::SignedTransaction
        sign_multi_agent_script_bytes_code(size_t account_index,
                                           std::vector<uint8_t> script,
                                           std::vector<diem_types::TypeTag> type_tags,
                                           std::vector<diem_types::TransactionArgument> args,
                                           std::vector<diem_types::AccountAddress> secondary_signer_addresses,                                           
                                           uint64_t max_gas_amount = 1'000'000,
                                           uint64_t gas_unit_price = 0,
                                           std::string_view gas_currency_code = "VLS",
                                           uint64_t expiration_timestamp_secs = 100) = 0;
        /**
         * @brief Submit a raw transaction with multi agent signatures
         *
         * @param account_index
         * @param secondary_signer_addresses
         * @param secondary_signers
         * @param raw_txn
         * @param max_gas_amount
         * @param gas_unit_price
         * @param gas_currency_code
         * @param expiration_timestamp_secs
         * @return uint64_t     the sequence number of transactions of sender account
         */
        virtual uint64_t
        submit_multi_agnet_raw_txn(size_t account_index,
                                   std::vector<diem_types::AccountAddress> secondary_signer_addresses,
                                   std::vector<diem_types::AccountAuthenticator> secondary_signers,
                                   const diem_types::RawTransaction &raw_txn,
                                   uint64_t max_gas_amount = 1'000'000,
                                   uint64_t gas_unit_price = 0,
                                   std::string_view gas_currency_code = "VLS",
                                   uint64_t expiration_timestamp_secs = 100) = 0;
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
        publish_module(size_t account_index,
                       std::vector<uint8_t> module) = 0;

        ////////////////////////////////////////////////////////////////
        // Methods for Violas framework
        ////////////////////////////////////////////////////////////////
        virtual void
        add_currency(size_t account_index, std::string_view currency_code) = 0;

        virtual uint64_t
        create_parent_vasp_account(const diem_types::AccountAddress &address,
                                   const std::array<uint8_t, 32> &auth_key,
                                   std::string_view human_name,
                                   bool add_all_currencies) = 0;

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const diem_types::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  std::string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) = 0;
    };

    using client2_ptr = std::shared_ptr<Client2>;

    void run_test_case(std::string_view url,
                       uint8_t chain_id,
                       std::string_view mint_key,
                       std::string_view mnemonic);
}