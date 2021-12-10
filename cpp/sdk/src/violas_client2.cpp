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

namespace dt = diem_types;

using namespace std;
using ta = diem_types::TransactionArgument;

namespace violas
{
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

        diem_types::TypeTag make_struct_type_tag(diem_types::AccountAddress address, string_view module, string_view name)
        {
            return diem_types::TypeTag{diem_types::TypeTag::Struct{address, diem_types::Identifier{string{module}}, diem_types::Identifier{string{name}}}};
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

        virtual tuple<size_t, diem_types::AccountAddress>
        create_next_account() override
        {
            auto index_address = m_wallet->create_next_account();
            auto [index, address] = index_address;

            auto opt_account_view = m_rpc_cli->get_account(address);
            if (opt_account_view.has_value())
                m_accounts[index] = *opt_account_view;

            return index_address;
        }

        virtual std::vector<Wallet::Account>
        get_all_accounts() override
        {
            return m_wallet->get_all_accounts();
        }
        //
        // submit a script and return sequence number of sender's account
        //
        uint64_t
        submit_txn_paylod(size_t account_index,
                          diem_types::TransactionPayload &&txn_paylod,
                          uint64_t max_gas_amount = 1'000'000,
                          uint64_t gas_unit_price = 0,
                          std::string_view gas_currency_code = "VLS",
                          uint64_t expiration_timestamp_secs = 100)
        {
            using namespace diem_types;
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
            raw_txn.chain_id = diem_types::ChainId{m_chain_id};
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

            m_rpc_cli->submit(signed_txn);

            // return the current sequence number and then increment it
            return iter->second.sequence_number++;
        }

        uint64_t
        submit_script(size_t account_index,
                      diem_types::Script &&script,
                      uint64_t max_gas_amount = 1'000'000,
                      uint64_t gas_unit_price = 0,
                      std::string_view gas_currency_code = "VLS",
                      uint64_t expiration_timestamp_secs = 100)
        {
            return this->submit_txn_paylod(
                account_index,
                {diem_types::TransactionPayload::Script{script}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
        }

        uint64_t
        submit_module(size_t account_index,
                      diem_types::Module &&module,
                      uint64_t max_gas_amount = 1'000'000,
                      uint64_t gas_unit_price = 0,
                      std::string_view gas_currency_code = "VLS",
                      uint64_t expiration_timestamp_secs = 100)
        {
            return this->submit_txn_paylod(
                account_index,
                {diem_types::TransactionPayload::Module{module}},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
        }

        void check_txn_vm_status(const diem_types::AccountAddress &address, uint64_t sequence_number, string_view error_info) override
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
                               [](VMStatus::OutOfGas status)
                               {
                                   __throw_runtime_error(status.type.c_str());
                               },
                               [=](VMStatus::MoveAbort status)
                               {
                                   ostringstream oss;

                                   oss << error_info << " error, "
                                       << "vm_status : { "
                                       << "type : " << status.type << ", "
                                       << "abort code : " << status.abort_code
                                       << " }"
                                       << endl;
                                   __throw_runtime_error(oss.str().c_str());
                               }},
                    opt_txn_view->vm_status.value);
            }
            else
                __throw_runtime_error("check_txn_vm_status is timeout.");
        }

        virtual uint64_t
        submit_script_byte_code(size_t account_index,
                                std::vector<uint8_t> script_byte_code,
                                std::vector<diem_types::TypeTag> type_tags,
                                std::vector<diem_types::TransactionArgument> args,
                                uint64_t max_gas_amount,
                                uint64_t gas_unit_price,
                                std::string_view gas_currency_code,
                                uint64_t expiration_timestamp_secs) override
        {
            using namespace diem_types;

            return this->submit_txn_paylod(
                account_index,
                {TransactionPayload::Script({diem_types::Script({script_byte_code, type_tags, args})})},
                max_gas_amount,
                gas_unit_price,
                gas_currency_code,
                expiration_timestamp_secs);
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
        virtual diem_types::SignedTransaction
        sign_multi_agent_script(size_t account_index,
                                diem_types::Script &&script,
                                std::vector<diem_types::AccountAddress> secondary_signer_addresses,
                                uint64_t max_gas_amount = 1'000'000,
                                uint64_t gas_unit_price = 0,
                                std::string_view gas_currency_code = "VLS",
                                uint64_t expiration_timestamp_secs = 100) override
        {
            using namespace diem_types;

            SignedTransaction signed_txn;
            RawTransaction &raw_txn = signed_txn.raw_txn;

            // Set transaction payload
            raw_txn.payload.value = TransactionPayload::Script{script};
            raw_txn.max_gas_amount = max_gas_amount;
            raw_txn.gas_unit_price = gas_unit_price;
            raw_txn.gas_currency_code = gas_currency_code;
            raw_txn.expiration_timestamp_secs = time(nullptr) + expiration_timestamp_secs;
            raw_txn.chain_id = diem_types::ChainId{m_chain_id};

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
        virtual std::tuple<diem_types::AccountAddress, uint64_t>
        sign_and_submit_multi_agent_signed_txn(size_t account_index,
                                               diem_types::SignedTransaction &&signed_txn) override
        {
            using namespace diem_types;
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
            serde::Serializable<std::vector<diem_types::AccountAddress>>::serialize(secondary_signer_addresses, serializer);
            bytes = std::move(serializer).bytes();
            copy(begin(bytes), end(bytes), back_insert_iterator(message));

            // Set

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

                multi_agent_auth.secondary_signer_addresses.push_back(m_accounts[account_index].address);
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
                       std::vector<uint8_t> module_bytes_code) override
        {
            this->submit_module(account_index, {module_bytes_code});
        }
        //
        //
        //
        virtual void
        add_currency(size_t account_index, std::string_view currency_code) override
        {
            auto sn = submit_script(account_index,
                                    diem_framework::encode_add_currency_to_account_script(
                                        make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)));

            this->check_txn_vm_status(m_accounts[account_index].address,
                                      sn,
                                      "add_currency");
        }

        virtual void
        allow_custom_script(bool is_allowing) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 6, 7, 13, 48, 8, 61, 16, 0, 0, 0, 1, 2, 1, 0, 1, 12, 0, 1, 6, 12, 31, 68, 105, 101, 109, 84, 114, 97, 110, 115, 97, 99, 116, 105, 111, 110, 80, 117, 98, 108, 105, 115, 104, 105, 110, 103, 79, 112, 116, 105, 111, 110, 15, 115, 101, 116, 95, 111, 112, 101, 110, 95, 115, 99, 114, 105, 112, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 14, 0, 17, 0, 2};

            auto sn = submit_script(ACCOUNT_ROOT_ID,
                                    diem_types::Script{script_bytecode, {}, {}});
            this->check_txn_vm_status(m_accounts[ACCOUNT_ROOT_ID].address,
                                      sn,
                                      "add_currency");
        }

        virtual uint64_t
        create_parent_vasp_account(
            const diem_types::AccountAddress &address,
            const std::array<uint8_t, 32> &auth_key,
            string_view human_name,
            bool add_all_currencies) override
        {
            if (m_opt_tc == std::nullopt)
                __throw_runtime_error("TC account is null, please specify the mint key file.");

            uint64_t sn = this->submit_script(
                ACCOUNT_TC_ID,
                diem_framework::encode_create_parent_vasp_account_script(
                    make_struct_type_tag(STD_LIB_ADDRESS, "VLS", "VLS"),
                    0,
                    address,
                    vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                    vector<uint8_t>(human_name.data(), human_name.data() + human_name.size()),
                    add_all_currencies));

            this->check_txn_vm_status(TC_ADDRESS, sn, "create_parent_vasp_account");

            return sn;
        }

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const diem_types::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) override
        {
            uint64_t sn = this->submit_script(
                account_index,
                diem_framework::encode_create_child_vasp_account_script(
                    make_struct_type_tag(STD_LIB_ADDRESS, currency, currency),
                    address,
                    vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                    add_all_currencies,
                    child_initial_balance));

            this->check_txn_vm_status(m_accounts[account_index].address, sn, "create_child_vasp_account");
        }

        virtual void
        create_designated_dealer_ex(uint64_t sliding_nonce,
                                    const diem_types::AccountAddress &address,
                                    const std::array<uint8_t, 32> &auth_key,
                                    std::string_view human_name,
                                    bool add_all_currencies) override
        {
            bytes script_bytecode = {161, 28, 235, 11, 3, 0, 0, 0, 6, 1, 0, 4, 3, 4, 11, 4, 15, 2, 5, 17, 26, 7, 43, 75, 8, 118, 16, 0, 0, 0, 1, 1, 2, 2, 1, 0, 0, 3, 4, 1, 1, 0, 1, 3, 6, 12, 3, 5, 10, 2, 10, 2, 1, 0, 2, 6, 12, 3, 1, 9, 0, 5, 6, 12, 5, 10, 2, 10, 2, 1, 11, 68, 105, 101, 109, 65, 99, 99, 111, 117, 110, 116, 12, 83, 108, 105, 100, 105, 110, 103, 78, 111, 110, 99, 101, 21, 114, 101, 99, 111, 114, 100, 95, 110, 111, 110, 99, 101, 95, 111, 114, 95, 97, 98, 111, 114, 116, 27, 99, 114, 101, 97, 116, 101, 95, 100, 101, 115, 105, 103, 110, 97, 116, 101, 100, 95, 100, 101, 97, 108, 101, 114, 95, 101, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 0, 1, 10, 14, 0, 10, 1, 17, 0, 14, 0, 10, 2, 11, 3, 11, 4, 10, 5, 56, 0, 2};

            auto sn = submit_script(ACCOUNT_ROOT_ID,
                                    diem_types::Script{script_bytecode,
                                                       {make_struct_type_tag(STD_LIB_ADDRESS, "VLS", "VLS")},
                                                       {
                                                           {ta::U64{sliding_nonce}},
                                                           {ta::Address{address}},
                                                           {ta::U8Vector{bytes(begin(auth_key), end(auth_key))}},
                                                           {ta::U8Vector{bytes(begin(human_name), end(human_name))}},
                                                           {ta::Bool{add_all_currencies}},
                                                       }});

            this->check_txn_vm_status(m_accounts[ACCOUNT_ROOT_ID].address,
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

            this->publish_module(ACCOUNT_ROOT_ID, module_bytes_code);
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
            auto script = diem_types::Script{
                script_bytecode,
                {make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)},
                {
                    {ta::U64{exchange_rate_denom}},
                    {ta::U64{exchange_rate_num}},
                    {ta::U64{scaling_factor}},
                    {ta::U64{fractional_part}},
                    {ta::U8Vector{bytes(begin(currency_code), end(currency_code))}},
                }};

            auto signed_txn = this->sign_multi_agent_script(
                ACCOUNT_ROOT_ID,
                move(script),
                {TC_ADDRESS});

            auto [sender, sn] = this->sign_and_submit_multi_agent_signed_txn(
                ACCOUNT_TC_ID,
                move(signed_txn));

            check_txn_vm_status(sender, sn, "sign_and_submit_multi_agent_signed_txn");
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