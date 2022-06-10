#include <fstream>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <diem_framework.hpp>
#include <optional>
#include <thread>
#include <chrono>

#include <utils.hpp>
#include "../include/violas_client2.hpp"

#include "../include/json_rpc.hpp"
#include "wallet.hpp"

using namespace std;
using namespace crypto;

namespace violas
{
    AccountState2::AccountState2(const std::string &hex)
    {
        auto bytes = hex_to_bytes(hex);

        // Deserialize to vector
        vector<uint8_t> data;
        {
            BcsSerde serde(move(bytes));
            serde &&data;
        }

        BcsSerde serde(move(data));

        serde &&_resources;
    }

    class Client2Imp : public Client2
    {
    private:
        json_rpc::client_ptr m_rpc_cli;
        uint8_t m_chain_id;

        shared_ptr<Wallet> m_wallet;

        // Account Root, Treasure Complaince, Test Designated Dealer
        optional<ed25519::PrivateKey> m_opt_root, m_opt_tc, m_opt_dd;

        map<size_t, json_rpc::AccountView> m_accounts;

        uint64_t get_sequence_number(size_t account_index)
        {
            auto accounts = m_wallet->get_all_accounts();
            return m_rpc_cli->get_account(accounts[account_index].address)->sequence_number;
        }

    public:
        Client2Imp(std::string_view url,
                   uint8_t chain_id,
                   std::string_view mnemonic_file,
                   std::string_view mint_key_file)
        {
            m_rpc_cli = json_rpc::Client::create(url);
            m_chain_id = chain_id;

            ifstream ifs(mnemonic_file.data());
            if (ifs.fail())
            {
                m_wallet = make_shared<Wallet>(Wallet::generate_from_random());

                ofstream ofs(mnemonic_file.data());
                ofs << m_wallet->export_mnemonic() << ";0"; // the ";0" is compatible with diem cli
            }
            else
            {
                istreambuf_iterator<char> is_iter(ifs);
                string mnemonic(is_iter, {});

                auto iter = mnemonic.find(';');
                if (iter != string::npos)
                    mnemonic.erase(iter);

                m_wallet = make_shared<Wallet>(Wallet::generate_from_mnemonic(mnemonic));
            }

            // intialize account Root, TC and DD
            if (!mint_key_file.empty())
            {
                ifstream ifs_mint_key(mint_key_file.data());
                if (!ifs_mint_key.fail())
                {
                    istreambuf_iterator<char> isb_iter(ifs_mint_key);
                    vector<uint8_t> buf(isb_iter, {});
                    ed25519::RawKey raw_key;

                    if (buf[0] != 32 && buf.size() != 33)
                        __throw_runtime_error("The size of mint key file is not 32, cannot handle it.");

                    copy(begin(buf) + 1, begin(buf) + 33, begin(raw_key));

                    m_opt_root = m_opt_tc = m_opt_dd = ed25519::PrivateKey::from_raw_key(raw_key);

                    auto root_account_view = m_rpc_cli->get_account(ROOT_ADDRESS);
                    if (root_account_view.has_value())
                        m_accounts[ACCOUNT_ROOT_ID] = *root_account_view;

                    auto tc_account_view = m_rpc_cli->get_account(TC_ADDRESS);
                    if (tc_account_view.has_value())
                        m_accounts[ACCOUNT_TC_ID] = *tc_account_view;

                    auto dd_account_view = m_rpc_cli->get_account(TESTNET_DD_ADDRESS);
                    if (dd_account_view.has_value())
                        m_accounts[ACCOUNT_DD_ID] = *dd_account_view;
                }
            }
        }

        ~Client2Imp()
        {
        }

        virtual tuple<size_t, dt::AccountAddress>
        create_next_account(std::optional<dt::AccountAddress> opt_address = std::nullopt) override
        {
            auto index_address = m_wallet->create_next_account();
            auto [index, address] = index_address;

            if (opt_address.has_value())
                address = *opt_address;

            auto opt_account_view = m_rpc_cli->get_account(address);
            if (opt_account_view.has_value())
                m_accounts[index] = *opt_account_view;
            else
            {
                json_rpc::AccountView view;
                view.address = address;
                view.sequence_number = 0;

                m_accounts[index] = view;
            }

            return make_tuple<>(index, address);
        }

        virtual void
        update_account_info(size_t account_index) override
        {
        }

        virtual std::vector<Wallet::Account>
        get_all_accounts() override
        {
            return m_wallet->get_all_accounts();
        }
        //
        // make a signed transaction
        //
        dt::SignedTransaction
        make_signed_txn(size_t account_index,
                        dt::TransactionPayload &&txn_paylod,
                        uint64_t max_gas_amount = 1'000'000,
                        uint64_t gas_unit_price = 0,
                        std::string_view gas_currency_code = "VLS",
                        uint64_t expiration_timestamp_secs = 100)
        {
            using namespace dt;
            SignedTransaction signed_txn;
            RawTransaction &raw_txn = signed_txn.raw_txn;

            raw_txn.payload = txn_paylod;

            auto iter = m_accounts.find(account_index);
            if (iter != end(m_accounts))
            {
                raw_txn.sequence_number = iter->second.sequence_number;
                raw_txn.sender = iter->second.address;
            }
            else if (auto opt_account_view = m_rpc_cli->get_account(m_wallet->get_all_accounts().at(account_index).address);
                     opt_account_view.has_value())
            {
                m_accounts[account_index] = *opt_account_view;
                raw_txn.sequence_number = opt_account_view->sequence_number;
                raw_txn.sender = opt_account_view->address;
                iter = m_accounts.find(account_index);
            }
            else
                __throw_runtime_error("Account index does not exist.");

            raw_txn.max_gas_amount = max_gas_amount;
            raw_txn.gas_unit_price = gas_unit_price;
            raw_txn.gas_currency_code = gas_currency_code;
            raw_txn.expiration_timestamp_secs = time(nullptr) + expiration_timestamp_secs;
            raw_txn.chain_id = dt::ChainId{m_chain_id};
            if (account_index == ACCOUNT_ROOT_ID)
                raw_txn.sender = ROOT_ADDRESS;
            else if (account_index == ACCOUNT_TC_ID)
                raw_txn.sender = TC_ADDRESS;
            else if (account_index == ACCOUNT_DD_ID)
                raw_txn.sender = TESTNET_DD_ADDRESS;

            auto bytes = raw_txn.bcsSerialize();

            string_view flag = "DIEM::RawTransaction";
            auto hash = sha3_256((uint8_t *)flag.data(), flag.size());

            vector<uint8_t> message(begin(hash), end(hash));
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            if (account_index == ACCOUNT_ROOT_ID)
            {
                ed25519::Signature signature = m_opt_root->sign(message.data(), message.size());
                signed_txn.authenticator.value = TransactionAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_root->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }
            else if (account_index == ACCOUNT_TC_ID)
            {
                auto priv_key = m_opt_tc->get_public_key().get_raw_key();

                ed25519::Signature signature = m_opt_tc->sign(message.data(), message.size());
                signed_txn.authenticator.value = TransactionAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};

                // bool ret = m_opt_tc->get_public_key().verify(signature, message.data(), message.size());
            }
            else if (account_index == ACCOUNT_DD_ID)
            {
                ed25519::Signature signature = m_opt_dd->sign(message.data(), message.size());
                signed_txn.authenticator.value = TransactionAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }
            else
            {
                auto priv_key = m_wallet->get_account_priv_key(account_index);

                ed25519::Signature signature = priv_key.sign(message.data(), message.size());

                signed_txn.authenticator.value = TransactionAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(priv_key.get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }

            return signed_txn;
        }

        //
        // submit a script and return sequence number of sender's account
        //
        std::tuple<dt::AccountAddress, uint64_t>
        submit_txn_payload(size_t account_index,
                           dt::TransactionPayload &&txn_paylod,
                           uint64_t max_gas_amount = 1'000'000,
                           uint64_t gas_unit_price = 0,
                           std::string_view gas_currency_code = "VLS",
                           uint64_t expiration_timestamp_secs = 100)
        {
            auto signed_txn = make_signed_txn(account_index,
                                              move(txn_paylod),
                                              max_gas_amount,
                                              gas_unit_price,
                                              gas_currency_code,
                                              expiration_timestamp_secs);

            m_rpc_cli->submit(signed_txn);

            auto iter = m_accounts.find(account_index);
            if (iter != end(m_accounts))
                iter->second.sequence_number++;

            // return the current sequence number and then increment it
            return make_tuple<>(signed_txn.raw_txn.sender, signed_txn.raw_txn.sequence_number);
        }

        Task<std::tuple<dt::AccountAddress, uint64_t>>
        await_submit_txn_payload(size_t account_index,
                                 dt::TransactionPayload &&txn_paylod,
                                 uint64_t max_gas_amount = 1'000'000,
                                 uint64_t gas_unit_price = 0,
                                 std::string_view gas_currency_code = "VLS",
                                 uint64_t expiration_timestamp_secs = 100)
        {
            auto signed_txn = make_signed_txn(account_index,
                                              move(txn_paylod),
                                              max_gas_amount,
                                              gas_unit_price,
                                              gas_currency_code,
                                              expiration_timestamp_secs);

            auto result = make_tuple(signed_txn.raw_txn.sender, signed_txn.raw_txn.sequence_number);

            co_await m_rpc_cli->await_submit2(move(signed_txn));

            auto iter = m_accounts.find(account_index);
            if (iter != end(m_accounts))
                iter->second.sequence_number++;

            co_return result;
        }

        std::tuple<dt::AccountAddress, uint64_t>
        submit_script(size_t account_index,
                      dt::Script &&script,
                      uint64_t max_gas_amount = 1'000'000,
                      uint64_t gas_unit_price = 0,
                      std::string_view gas_currency_code = "VLS",
                      uint64_t expiration_timestamp_secs = 100)
        {
            return this->submit_txn_payload(
                account_index,
                {dt::TransactionPayload::Script{script}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
        }

        virtual Task<std::tuple<dt::AccountAddress, uint64_t>>
        await_submit_script(size_t account_index,
                            dt::Script &&script,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100)
        {
            auto ret = co_await await_submit_txn_payload(
                account_index,
                {dt::TransactionPayload::Script{script}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);

            co_return ret;
        }

        std::tuple<dt::AccountAddress, uint64_t>
        submit_module(size_t account_index,
                      dt::Module &&module,
                      uint64_t max_gas_amount = 1'000'000,
                      uint64_t gas_unit_price = 0,
                      std::string_view gas_currency_code = "VLS",
                      uint64_t expiration_timestamp_secs = 100)
        {
            return this->submit_txn_payload(
                account_index,
                {dt::TransactionPayload::Module{module}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
        }

        Task<std::tuple<dt::AccountAddress, uint64_t>>
        await_submit_module(size_t account_index,
                            dt::Module &&module,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100)
        {
            auto ret = co_await this->await_submit_txn_payload(
                account_index,
                {dt::TransactionPayload::Module{module}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);

            co_return ret;
        }

        void check_txn_vm_status(const dt::AccountAddress &address, uint64_t sequence_number, string_view error_info) override
        {
            using namespace json_rpc;
            optional<TransactionView> opt_txn_view = nullopt;

            for (int i = 0; i < 50; i++)
            {
                opt_txn_view = m_rpc_cli->get_account_transaction(address, sequence_number, false);
                if (opt_txn_view.has_value())
                    break;

                this_thread::sleep_for(chrono::milliseconds(100)); // 0.1 second
            }

            if (opt_txn_view.has_value())
            {
                std::visit(
                    overloaded{[](VMStatus::Executed status) {},
                               [=](VMStatus::ExecutionFailure status)
                               {
                                   ostringstream oss;

                                   oss << error_info << " error, "
                                       << "vm_status : {\n"
                                       << "type : " << status.type << ", "
                                       << "location : " << status.location << ", \n"
                                       << "function index : " << status.function_index << ", \n"
                                       << "code offset : " << status.code_offset << ", \n"
                                       << "}";

                                   __throw_runtime_error(oss.str().c_str());
                               },
                               [](VMStatus::OutOfGas status)
                               {
                                   __throw_runtime_error(status.type.c_str());
                               },
                               [=](VMStatus::MiscellaneousError status)
                               {
                                   ostringstream oss;
                                   oss << error_info << " failed, VM status : " << status.type;

                                   __throw_runtime_error(oss.str().c_str());
                               },
                               [=](VMStatus::MoveAbort status)
                               {
                                   ostringstream oss;

                                   oss << error_info << " error, "
                                       << "vm_status : { "
                                       << "type : " << status.type << ", "
                                       << "abort code : " << status.abort_code
                                       << " }";

                                   __throw_runtime_error(oss.str().c_str());
                               }},
                    opt_txn_view->vm_status.value);
            }
            else
                __throw_runtime_error("check_txn_vm_status is timeout.");
        }

        Task<void>
        await_check_txn_vm_status(const dt::AccountAddress &address,
                                  uint64_t sequence_number,
                                  std::string_view error_info) override
        {
            using namespace json_rpc;
            optional<TransactionView> opt_txn_view = nullopt;

            for (int i = 0; i < 50; i++)
            {
                opt_txn_view = co_await m_rpc_cli->await_get_account_transaction(address, sequence_number, false);
                if (opt_txn_view.has_value())
                    break;

                this_thread::sleep_for(chrono::milliseconds(100)); // 0.1 second
            }

            if (opt_txn_view.has_value())
            {
                std::visit(
                    overloaded{[](VMStatus::Executed status) {},
                               [=](VMStatus::ExecutionFailure status)
                               {
                                   ostringstream oss;

                                   oss << error_info << " error, "
                                       << "vm_status : {\n"
                                       << "type : " << status.type << ", "
                                       << "location : " << status.location << ", \n"
                                       << "function index : " << status.function_index << ", \n"
                                       << "code offset : " << status.code_offset << ", \n"
                                       << "}";

                                   __throw_runtime_error(oss.str().c_str());
                               },
                               [](VMStatus::OutOfGas status)
                               {
                                   __throw_runtime_error(status.type.c_str());
                               },
                               [=](VMStatus::MiscellaneousError status)
                               {
                                   ostringstream oss;
                                   oss << error_info << " failed, VM status : " << status.type;

                                   __throw_runtime_error(oss.str().c_str());
                               },
                               [=](VMStatus::MoveAbort status)
                               {
                                   ostringstream oss;

                                   oss << error_info << " error, "
                                       << "vm_status : { "
                                       << "type : " << status.type << ", "
                                       << "abort code : " << status.abort_code
                                       << " }";

                                   __throw_runtime_error(oss.str().c_str());
                               }},
                    opt_txn_view->vm_status.value);
            }
            else
                __throw_runtime_error("check_txn_vm_status is timeout.");
        }

        virtual std::tuple<dt::AccountAddress, uint64_t>
        execute_script_bytecode(size_t account_index,
                                std::vector<uint8_t> script_byte_code,
                                std::vector<dt::TypeTag> type_tags,
                                std::vector<dt::TransactionArgument> args,
                                uint64_t max_gas_amount,
                                uint64_t gas_unit_price,
                                std::string_view gas_currency_code,
                                uint64_t expiration_timestamp_secs) override
        {
            return this->submit_txn_payload(
                account_index,
                {dt::TransactionPayload::Script({dt::Script({script_byte_code, type_tags, args})})},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
        }

        virtual std::tuple<dt::AccountAddress, uint64_t>
        execute_script_file(size_t account_index,
                            std::string_view script_file_name,
                            std::vector<dt::TypeTag> type_tags,
                            std::vector<dt::TransactionArgument> args,
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string_view gas_currency_code = "VLS",
                            uint64_t expiration_timestamp_secs = 100) override
        {
            ifstream ifs(script_file_name.data(), ios::binary);

            if (!ifs.is_open())
                throw runtime_error(fmt("failed to open file ", script_file_name, " at execute_script_file"));

            bytes script_bytecode(istreambuf_iterator<char>(ifs), {});
            dt::Script script{
                script_bytecode,
                type_tags,
                args,
            };

            return this->submit_script(account_index, move(script), max_gas_amount, gas_unit_price, gas_currency_code, expiration_timestamp_secs);
        }

        virtual Task<std::tuple<dt::AccountAddress, uint64_t>>
        await_execute_script(size_t account_index,
                             std::string_view script_file_name,
                             std::vector<dt::TypeTag> &&type_tags,
                             std::vector<dt::TransactionArgument> &&args,
                             uint64_t max_gas_amount = 1'000'000,
                             uint64_t gas_unit_price = 0,
                             std::string_view gas_currency_code = "VLS",
                             uint64_t expiration_timestamp_secs = 100) override
        {
            // struct awaitable
            // {
            //     Client2Imp &_client;
            //     size_t account_index;
            //     std::string_view script_file_name;
            //     std::vector<dt::TypeTag> type_tags;
            //     std::vector<dt::TransactionArgument> args;
            //     uint64_t max_gas_amount = 1'000'000;
            //     uint64_t gas_unit_price = 0;
            //     std::string_view gas_currency_code = "VLS";
            //     uint64_t expiration_timestamp_secs = 100;

            //     dt::AccountAddress _sender_address;
            //     uint64_t _sequence_number;

            //     bool await_ready() { return false; }
            //     auto await_resume() { return std::make_tuple<>(_sender_address, _sequence_number); }
            //     void await_suspend(std::coroutine_handle<> h)
            //     {
            //         ifstream ifs(script_file_name.data(), ios::binary);

            //         if (!ifs.is_open())
            //             throw runtime_error(fmt("failed to open file ", script_file_name, " at execute_script_file"));

            //         bytes script_bytecode(istreambuf_iterator<char>(ifs), {});
            //         dt::Script script{
            //             script_bytecode,
            //             type_tags,
            //             args,
            //         };

            //         _client.async_submit_script(account_index, move(script),
            //                                     max_gas_amount, gas_unit_price, gas_currency_code,
            //                                     expiration_timestamp_secs,
            //                                     [h, this](dt::AccountAddress address, uint64_t sn) mutable
            //                                     {
            //                                         this->_sender_address = address;
            //                                         this->_sequence_number = sn;
            //                                         h.resume();
            //                                     });
            //     }
            // };

            // auto ret = co_await awaitable{*this, account_index, script_file_name, std::move(type_tags), std::move(args)};

            ifstream ifs(script_file_name.data(), ios::binary);

            if (!ifs.is_open())
                throw runtime_error(fmt("failed to open file ", script_file_name, " at execute_script_file"));

            bytes script_bytecode(istreambuf_iterator<char>(ifs), {});
            dt::Script script{
                script_bytecode,
                type_tags,
                args,
            };

            auto ret = co_await await_submit_script(account_index,
                                                    move(script),
                                                    max_gas_amount,
                                                    gas_unit_price,
                                                    gas_currency_code,
                                                    expiration_timestamp_secs);

            co_return ret;
        }

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
                                uint64_t expiration_timestamp_secs = 100) override
        {
            using namespace dt;

            SignedTransaction signed_txn;
            RawTransaction &raw_txn = signed_txn.raw_txn;

            // Set transaction payload
            raw_txn.payload.value = TransactionPayload::Script{script};
            raw_txn.max_gas_amount = max_gas_amount;
            raw_txn.gas_unit_price = gas_unit_price;
            raw_txn.gas_currency_code = gas_currency_code;
            raw_txn.expiration_timestamp_secs = time(nullptr) + expiration_timestamp_secs;
            raw_txn.chain_id = dt::ChainId{m_chain_id};

            // Set sender and sequence number
            auto iter = m_accounts.find(account_index);
            if (iter != end(m_accounts))
            {
                raw_txn.sender = iter->second.address;
                raw_txn.sequence_number = iter->second.sequence_number;
            }
            else if (auto opt_account_view = m_rpc_cli->get_account(m_wallet->get_all_accounts().at(account_index).address);
                     opt_account_view.has_value())
            {
                raw_txn.sender = opt_account_view->address;
                raw_txn.sequence_number = opt_account_view->sequence_number;

                // Update m_accounts and iter
                m_accounts[account_index] = *opt_account_view;
                iter = m_accounts.find(account_index);
            }
            else
                __throw_runtime_error("Account index does not exist.");

            // Sign for flag + raw transaction + secondary_signer_addresses
            string_view flag = "DIEM::RawTransactionWithData";
            auto hash = sha3_256((uint8_t *)flag.data(), flag.size());

            vector<uint8_t> message(begin(hash), end(hash));

            // RawTransactionWithData enum 0
            message.push_back(0);

            auto bytes = raw_txn.bcsSerialize();
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            // Serialize secondary signer addresses
            auto serializer = serde::BcsSerializer();
            serde::Serializable<vector<AccountAddress>>::serialize(secondary_signer_addresses, serializer);
            bytes = std::move(serializer).bytes();
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            // Set multi agent authenticators
            TransactionAuthenticator::MultiAgent multi_agent_auth;
            multi_agent_auth.secondary_signer_addresses = secondary_signer_addresses;

            // Set sender's authenticator
            if (account_index == ACCOUNT_ROOT_ID)
            {
                ed25519::Signature signature = m_opt_root->sign(message.data(), message.size());
                multi_agent_auth.sender.value = AccountAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_root->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }
            else if (account_index == ACCOUNT_TC_ID)
            {
                auto priv_key = m_opt_tc->get_public_key().get_raw_key();

                ed25519::Signature signature = m_opt_tc->sign(message.data(), message.size());
                multi_agent_auth.sender.value = AccountAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }
            else if (account_index == ACCOUNT_DD_ID)
            {
                ed25519::Signature signature = m_opt_dd->sign(message.data(), message.size());
                multi_agent_auth.sender.value = AccountAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }
            else
            {
                auto priv_key = m_wallet->get_account_priv_key(account_index);

                ed25519::Signature signature = priv_key.sign(message.data(), message.size());

                multi_agent_auth.sender.value = AccountAuthenticator::Ed25519{
                    Ed25519PublicKey{u8_array_to_vector(priv_key.get_public_key().get_raw_key())},
                    Ed25519Signature{u8_array_to_vector(signature)}};
            }

            // Set authenticator for signed txn
            signed_txn.authenticator.value = multi_agent_auth;

            return signed_txn;
        }

        /**
         * @brief Submit a multi agent signed transaction
         *
         * @param account_index
         * @param txn
         * @param secondary_signer_addresse
         * @return uint64_t
         */
        virtual std::tuple<dt::AccountAddress, uint64_t>
        sign_and_submit_multi_agent_signed_txn(size_t account_index,
                                               dt::SignedTransaction &&signed_txn) override
        {
            using namespace dt;
            const RawTransaction &raw_txn = signed_txn.raw_txn;
            auto &multi_agent_auth = get<TransactionAuthenticator::MultiAgent>(signed_txn.authenticator.value);
            auto &secondary_signer_addresses = multi_agent_auth.secondary_signer_addresses;

            // Sign for flag + raw transaction + secondary_signer_addresses
            string_view flag = "DIEM::RawTransactionWithData";
            auto hash = sha3_256((uint8_t *)flag.data(), flag.size());

            vector<uint8_t> message(begin(hash), end(hash));

            message.push_back(0);

            auto bytes = raw_txn.bcsSerialize();
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            // Serialize secondary signer addresses
            auto serializer = serde::BcsSerializer();
            serde::Serializable<std::vector<dt::AccountAddress>>::serialize(secondary_signer_addresses, serializer);
            bytes = std::move(serializer).bytes();
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            // Set sender's authenticator
            if (account_index == ACCOUNT_ROOT_ID)
            {
                ed25519::Signature signature = m_opt_root->sign(message.data(), message.size());
                multi_agent_auth.secondary_signer_addresses.push_back(m_accounts[account_index].address);

                multi_agent_auth.secondary_signers.push_back(
                    {AccountAuthenticator::Ed25519{
                        Ed25519PublicKey{u8_array_to_vector(m_opt_root->get_public_key().get_raw_key())},
                        Ed25519Signature{u8_array_to_vector(signature)}}});
            }
            else if (account_index == ACCOUNT_TC_ID)
            {
                ed25519::Signature signature = m_opt_tc->sign(message.data(), message.size());

                multi_agent_auth.secondary_signers.push_back(
                    {AccountAuthenticator::Ed25519{
                        Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                        Ed25519Signature{u8_array_to_vector(signature)}}});
            }
            else if (account_index == ACCOUNT_DD_ID)
            {
                ed25519::Signature signature = m_opt_dd->sign(message.data(), message.size());

                multi_agent_auth.secondary_signer_addresses.push_back(m_accounts[account_index].address);
                multi_agent_auth.secondary_signers.push_back(
                    {AccountAuthenticator::Ed25519{
                        Ed25519PublicKey{u8_array_to_vector(m_opt_tc->get_public_key().get_raw_key())},
                        Ed25519Signature{u8_array_to_vector(signature)}}});
            }
            else
            {
                auto priv_key = m_wallet->get_account_priv_key(account_index);
                ed25519::Signature signature = priv_key.sign(message.data(), message.size());

                // multi_agent_auth.secondary_signer_addresses.push_back(m_accounts[account_index].address);
                multi_agent_auth.secondary_signers.push_back(
                    {AccountAuthenticator::Ed25519{
                        Ed25519PublicKey{u8_array_to_vector(priv_key.get_public_key().get_raw_key())},
                        Ed25519Signature{u8_array_to_vector(signature)}}});
            }

            m_rpc_cli->submit(signed_txn);

            return make_tuple<>(raw_txn.sender, raw_txn.sequence_number);
        }

        virtual void
        publish_module(size_t account_index,
                       std::vector<uint8_t> &&module_bytes_code) override
        {
            auto [sender, sn] = this->submit_module(account_index, {module_bytes_code});

            this->check_txn_vm_status(sender, sn, "publish_module");
        }

        virtual Task<void>
        await_publish_module(size_t account_index,
                             std::vector<uint8_t> &&module_bytes_code) override
        {
            auto [sender, sn] = co_await this->await_submit_module(account_index, {module_bytes_code});

            co_await this->await_check_txn_vm_status(sender, sn, "publish_module");
        }

        virtual void
        publish_module(size_t account_index,
                       std::string_view module_file_name) override
        {
            ifstream ifs(module_file_name.data(), ios::binary);

            if (!ifs.is_open())
                throw runtime_error(format("failed to open file %s at submit_script_file", module_file_name.data()).c_str());

            this->publish_module(account_index, bytes(istreambuf_iterator<char>(ifs), {}));
        }

        virtual Task<void>
        await_publish_module(size_t account_index,
                             std::string_view module_file_name) override
        {
            ifstream ifs(module_file_name.data(), ios::binary);

            if (!ifs.is_open())
                //throw runtime_error(format("failed to open file %s at submit_script_file", module_file_name).c_str());
                throw runtime_error("module file cannot be open");

            co_await await_publish_module(account_index, bytes(istreambuf_iterator<char>(ifs), {}));            
        }

        virtual std::optional<AccountState2>
        get_account_state(const dt::AccountAddress address) override
        {
            json_rpc::AccountStateWithProof asp = m_rpc_cli->get_account_state_blob(bytes_to_hex(address.value));

            if (asp.blob.empty())
                return {};
            else
                return AccountState2(asp.blob);
        }

        virtual Task<std::optional<AccountState2>>
        await_get_account_state(const dt::AccountAddress address) override
        {
            json_rpc::AccountStateWithProof asp = co_await m_rpc_cli->await_get_account_state_blob(bytes_to_hex(address.value));

            if (asp.blob.empty())
                co_return {};
            else
                co_return AccountState2(asp.blob);
        }

        virtual std::vector<json_rpc::EventView>
        get_events(EventHandle handle, uint64_t start, uint64_t limit) override
        {
            return m_rpc_cli->get_events(bytes_to_hex(handle.guid), start, limit);
        }

        virtual Task<std::vector<json_rpc::EventView>>
        await_get_events(EventHandle handle, uint64_t start, uint64_t limit) override
        {
            co_return co_await m_rpc_cli->await_get_events(bytes_to_hex(handle.guid), start, limit);
        }

        //
        //
        //
        virtual void
        add_currency(size_t account_index, std::string_view currency_code) override
        {
            auto [sender, sn] = submit_script(account_index,
                                              diem_framework::encode_add_currency_to_account_script(
                                                  make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)));

            this->check_txn_vm_status(sender,
                                      sn,
                                      "add_currency");
        }

        virtual void
        allow_custom_script(bool is_allowing) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 48, 8, 61, 16, 0, 0, 0, 1, 2, 1, 0, 1, 12, 0, 1, 6, 12, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 115, 99, 114, 105, 112, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 14, 0, 17, 0, 2};

            auto [sender, sn] = submit_script(ACCOUNT_ROOT_ID,
                                              dt::Script{script_bytecode, {}, {}});

            this->check_txn_vm_status(sender,
                                      sn,
                                      "allow_custom_script");
        }

        virtual Task<void>
        await_allow_custom_script(bool is_allowing) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 48, 8, 61, 16, 0, 0, 0, 1, 2, 1, 0, 1, 12, 0, 1, 6, 12, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 115, 99, 114, 105, 112, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 14, 0, 17, 0, 2};

            auto [sender, sn] = co_await await_submit_script(ACCOUNT_ROOT_ID,
                                                             dt::Script{script_bytecode, {}, {}});

            co_await await_check_txn_vm_status(sender, sn, "await_allow_custom_script");
        }

        virtual void
        allow_publishing_module(bool is_allowing) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 8, 7, 15, 48, 8, 63, 16, 0, 0, 0, 1, 2, 1, 0, 2, 12, 1, 0, 2, 6, 12, 1, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 109, 111, 100, 117, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 14, 0, 10, 1, 17, 0, 2};

            auto [sender, sn] = this->submit_script(ACCOUNT_ROOT_ID, dt::Script{script_bytecode, {}, make_txn_args(is_allowing)});

            this->check_txn_vm_status(sender, sn, "allow_publishing_module");
        }

        virtual Task<void>
        await_allow_publishing_module(bool is_allowing) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 8, 7, 15, 48, 8, 63, 16, 0, 0, 0, 1, 2, 1, 0, 2, 12, 1, 0, 2, 6, 12, 1, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 109, 111, 100, 117, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 14, 0, 10, 1, 17, 0, 2};

            auto [sender, sn] = co_await await_submit_script(ACCOUNT_ROOT_ID, dt::Script{script_bytecode, {}, make_txn_args(is_allowing)});

            co_await await_check_txn_vm_status(sender, sn, "allow_publishing_module");
        }

        virtual uint64_t
        create_parent_vasp_account(
            const dt::AccountAddress &address,
            const std::array<uint8_t, 32> &auth_key,
            string_view human_name,
            bool add_all_currencies) override
        {
            if (m_opt_tc == std::nullopt)
                __throw_runtime_error("TC account is null, please specify the mint key file.");

            auto [sender, sn] = this->submit_script(
                ACCOUNT_TC_ID,
                diem_framework::encode_create_parent_vasp_account_script(
                    make_struct_type_tag(STD_LIB_ADDRESS, "VLS", "VLS"),
                    0,
                    address,
                    vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                    vector<uint8_t>(human_name.data(), human_name.data() + human_name.size()),
                    add_all_currencies));

            this->check_txn_vm_status(sender, sn, "create_parent_vasp_account");

            return sn;
        }

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const dt::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) override
        {
            auto [sender, sn] = this->submit_script(
                account_index,
                diem_framework::encode_create_child_vasp_account_script(
                    make_struct_type_tag(STD_LIB_ADDRESS, currency, currency),
                    address,
                    vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                    add_all_currencies,
                    child_initial_balance));

            this->check_txn_vm_status(sender, sn, "create_child_vasp_account");
        }

        virtual void
        create_designated_dealer_ex(std::string_view currency_code,
                                    uint64_t sliding_nonce,
                                    const dt::AccountAddress &address,
                                    const std::array<uint8_t, 32> &auth_key,
                                    std::string_view human_name,
                                    bool add_all_currencies) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 26, 7, 43, 75, 8, 118, 16, 0, 0, 0, 1, 1, 2, 2, 1, 0, 0, 3, 4, 1, 1, 0, 1, 3, 6, 12, 3, 5, 10, 2, 10, 2, 1, 0, 2, 6, 12, 3, 1, 9, 0, 5, 6, 12, 5, 10, 2, 10, 2, 1, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 27, 99, 114, 101, 97, 116, 101, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 95, 101, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 10, 14, 0, 10, 1, 17, 0, 14, 0, 10, 2, 11, 3, 11, 4, 10, 5, 56, 0, 2};

            auto [sender, sn] = this->submit_script(
                ACCOUNT_TC_ID,
                dt::Script{
                    script_bytecode,
                    {make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)},
                    make_txn_args(sliding_nonce, address, auth_key, human_name, add_all_currencies),
                });

            this->check_txn_vm_status(sender,
                                      sn,
                                      "create_designated_dealer_ex");
        }
        void publish_currency_module(std::string_view currency_code)
        {
            bytes module_bytes_code;

            if (currency_code.size() == 3)
            {
                uint8_t currency_module[] = {
                    161, 28, 235, 11, 2, 0, 0, 0, 5, 1, 0, 2, 2, 2, 4, 7, 6, 16, 8, 22, 16, 10, 38, 5,
                    0, 0, 0, 0, 4, 0, 3, 85, 83, 68, 11, 100, 117, 109, 109, 121, 95, 102, 105, 101,
                    108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 0};

                string_view sym = "USD";
                auto iter = search(begin(currency_module), end(currency_module), begin(sym), end(sym));
                if (iter != end(currency_module))
                {
                    // replace currency code with new code
                    copy(begin(currency_code), end(currency_code), iter);
                }

                module_bytes_code = move(bytes(begin(currency_module), end(currency_module)));
            }

            this->publish_module(ACCOUNT_ROOT_ID, move(module_bytes_code));
        }

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
                                  uint64_t fractional_part) override
        {
            this->publish_currency_module(currency_code);

            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 7, 1, 0, 6, 2, 6, 4, 3, 10, 17, 4, 27, 4, 5, 31, 33, 7, 64, 103, 8, 167, 1, 16, 0, 0, 0, 1, 0, 2, 2, 2, 7, 0, 2, 3, 3, 1, 0, 1, 4, 5, 2, 1, 0, 0, 5, 6, 2, 1, 0, 1, 4, 2, 4, 7, 12, 12, 3, 3, 3, 3, 10, 2, 1, 8, 0, 0, 2, 3, 3, 1, 9, 0, 6, 6, 12, 6, 12, 8, 0, 3, 3, 10, 2, 1, 6, 12, 13, 65, 99, 99, 111, 117, 110, 116, 76, 105, 109, 105, 116, 115, 4, 68, 105, 101, 109, 12, 70, 105, 120, 101, 100, 80, 111, 105, 110, 116, 51, 50, 20, 99, 114, 101, 97, 116, 101, 95, 102, 114, 111, 109, 95, 114, 97, 116, 105, 111, 110, 97, 108, 21, 114, 101, 103, 105, 115, 116, 101, 114, 95, 83, 67, 83, 95, 99, 117, 114, 114, 101, 110, 99, 121, 27, 112, 117, 98, 108, 105, 115, 104, 95, 117, 110, 114, 101, 115, 116, 114, 105, 99, 116, 101, 100, 95, 108, 105, 109, 105, 116, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 0, 1, 14, 10, 2, 10, 3, 17, 0, 12, 7, 14, 0, 14, 1, 11, 7, 10, 4, 10, 5, 11, 6, 56, 0, 14, 0, 56, 1, 2};
            auto script = dt::Script{
                script_bytecode,
                {make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)},
                make_txn_args(exchange_rate_denom,
                              exchange_rate_num,
                              scaling_factor,
                              fractional_part,
                              currency_code),
            };

            auto signed_txn = this->sign_multi_agent_script(
                ACCOUNT_ROOT_ID,
                move(script),
                {TC_ADDRESS});

            auto [sender, sn] = this->sign_and_submit_multi_agent_signed_txn(
                ACCOUNT_TC_ID,
                move(signed_txn));

            check_txn_vm_status(sender, sn, "regiester_stable_currency");
        }

        /**
         * @brief Add a currency for DD account, this mehtod needs TC account Permission
         *
         * @param dd_address DD account address
         */
        virtual void
        add_currency_for_designated_dealer(
            std::string_view currency_code,
            dt::AccountAddress dd_address) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 6, 1, 0, 2, 3, 2, 6, 4, 8, 2, 5, 10, 11, 7, 21, 47, 8, 68, 16, 0, 0, 0, 1, 3, 1, 1, 4, 0, 2, 2, 12, 5, 0, 1, 9, 0, 2, 6, 12, 5, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 34, 97, 100, 100, 95, 99, 117, 114, 114, 101, 110, 99, 121, 95, 102, 111, 114, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 0, 1, 4, 14, 0, 10, 1, 56, 0, 2};
            dt::Script script{
                script_bytecode,
                {make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)},
                make_txn_args(dd_address),
            };

            make_txn_args(__uint128_t(0));

            auto [sender, sn] = this->submit_script(ACCOUNT_TC_ID, move(script));
            check_txn_vm_status(sender, sn, "tc_add_currency_for_designated_dealer");
        }

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
             uint64_t tier_index) override
        {
            auto script = diem_framework::encode_tiered_mint_script(
                make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code),
                sliding_nonce,
                dd_address,
                amount,
                tier_index);

            auto [sender, sn] = this->submit_script(ACCOUNT_TC_ID, move(script));
            check_txn_vm_status(sender, sn, "mint");
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