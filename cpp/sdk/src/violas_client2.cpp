#include <fstream>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <diem_framework.hpp>
#include <optional>

#include <utils.hpp>
#include "../include/violas_client2.hpp"

#include "../include/json_rpc.hpp"
#include "wallet.hpp"

using namespace std;

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

        map<size_t, json_rpc::AccountView> m_account_infos;

        uint64_t get_sequence_number(size_t account_index)
        {
            auto accounts = m_wallet->get_all_accounts();
            return m_rpc_cli->get_account(accounts[account_index].address).sequence_number;
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
                    m_account_infos[ACCOUNT_ROOT_ID] = root_account_view;
                    auto tc_account_view = m_rpc_cli->get_account(TC_ADDRESS);
                    m_account_infos[ACCOUNT_TC_ID] = tc_account_view;
                    auto dd_account_view = m_rpc_cli->get_account(TESTNET_DD_ADDRESS);
                    m_account_infos[ACCOUNT_DD_ID] = dd_account_view;
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

            auto account_view = m_rpc_cli->get_account(address);
            m_account_infos[index] = account_view;

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
        submit_script(size_t account_index,
                      diem_types::Script &&script,
                      uint64_t max_gas_amount = 1'000'000,
                      uint64_t gas_unit_price = 0,
                      std::string_view gas_currency_code = "VLS",
                      uint64_t expiration_timestamp_secs = 100)
        {
            using namespace diem_types;
            SignedTransaction signed_txn;
            RawTransaction &raw_txn = signed_txn.raw_txn;

            raw_txn.payload.value = TransactionPayload::Script({std::move(script)});

            auto iter = m_account_infos.find(account_index);
            if (iter != end(m_account_infos))
            {
                raw_txn.sequence_number = iter->second.sequence_number;
                raw_txn.sender = iter->second.address;
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

            return iter->second.sequence_number++;
        }

        void check_vm_status(const json_rpc::VMStatus &vm_status, string_view info)
        {
            using VMStatus = json_rpc::VMStatus;

            std::visit(overloaded{[](VMStatus::Executed status) {},
                                  [](VMStatus::OutOfGas status)
                                  {
                                      __throw_runtime_error(status.type.c_str());
                                  },
                                  [=](VMStatus::MoveAbort status)
                                  {
                                      ostringstream oss;
                                      oss << info << " error, " 
                                      << "vm_status : {"
                                          << "type : " << status.type << " "
                                          << "abort code : " << status.abort_code
                                          << " }"
                                          << endl;
                                      __throw_runtime_error(oss.str().c_str());
                                  }},
                       vm_status.value);
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
                          diem_framework::encode_add_currency_to_account_script(
                              make_struct_type_tag(STD_LIB_ADDRESS, currency_code, currency_code)));
        }

        virtual void
        create_parent_vasp_account(
            const diem_types::AccountAddress &address,
            const std::array<uint8_t, 32> &auth_key,
            string_view human_name,
            bool add_all_currencies) override
        {
            if (m_opt_tc == std::nullopt)
                __throw_runtime_error("TC account is null, please specify the mint key file.");

            uint64_t sn = submit_script(ACCOUNT_TC_ID,
                                        diem_framework::encode_create_parent_vasp_account_script(
                                            make_struct_type_tag(STD_LIB_ADDRESS, "VLS", "VLS"),
                                            0,
                                            address,
                                            vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                                            vector<uint8_t>(human_name.data(), human_name.data() + human_name.size()),
                                            add_all_currencies));

            auto txn_view = m_rpc_cli->get_account_transaction(m_account_infos[ACCOUNT_TC_ID].address, sn, false);

            check_vm_status(txn_view.vm_status, "create_parent_vasp_account");
        }

        virtual void
        create_child_vasp_account(size_t account_index,
                                  const diem_types::AccountAddress &address,
                                  const std::array<uint8_t, 32> &auth_key,
                                  string_view currency,
                                  uint64_t child_initial_balance,
                                  bool add_all_currencies = false) override
        {
            submit_script(account_index,
                          diem_framework::encode_create_child_vasp_account_script(
                              make_struct_type_tag(STD_LIB_ADDRESS, currency, currency),
                              address,
                              vector<uint8_t>(begin(auth_key), begin(auth_key) + 16),
                              add_all_currencies,
                              child_initial_balance));
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