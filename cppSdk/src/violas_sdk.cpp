#include <ctime>
#include <iomanip>
//#include <chrono>
//#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <functional>

#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
#endif

#include "violas_sdk.hpp"
#include "rust_client_proxy.hpp"

using namespace std;

ostream &log(ostream &ost, const char *flag, const char *file, int line,
             const char *func)
{
    time_t now = time(nullptr);

    ost << flag << put_time(std::localtime(&now), "%F %T") << " (" << file << ":"
        << line << ":" << func << ") : ";

    return ost;
}

#define EXCEPTION_AT \
    format(", exception at (%s:%s:%d)", __FILE__, __func__, __LINE__)

namespace LIB_NAME
{
    template <size_t N>
    using bytes = array<uint8_t, N>;

    template <size_t N>
    ostream &operator<<(ostream &os, const bytes<N> &value)
    {
        for (auto v : value)
        {
            os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
        }

        os << std::dec;

        return os;
    }

    template <size_t N>
    std::ostream &operator>>(std::ostream &os, const bytes<N> &value) { return os; }

    template <size_t N>
    bytes<N> bytes_from_string(const std::string &str)
    {
        bytes<N> addr;
        auto iter = begin(addr);

        for (size_t i = 0; i < str.size() && i < 64; i += 2)
        {
            std::istringstream iss(str.substr(i, 2));
            uint32_t byte;

            iss >> std::hex >> std::setw(2) >> byte; // std::setfill('0') >> std::setw(2)
            *iter++ = byte;
        }

        return addr;
    }

    std::string tx_vec_data(const std::string &data)
    {
        ostringstream oss;
        oss << "b\"";

        for (auto v : data)
        {
            oss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
        }

        oss << "\"";

        return oss.str();
    }

    bool is_valid_balance(uint64_t value)
    {
#if __cplusplus >= 201703L
        const uint64_t max_uint64 = numeric_limits<uint64_t>::max();
#else
        const uint64_t max_uint64 = 0xFFFFFFFFFFFFFFFF;
#endif
        return value != max_uint64;
    }

    void transform_mv_to_json(const std::string &mv_file_name,
                              const std::string &json_file_name,
                              const bytes<16> &address)
    {
        ifstream mv(mv_file_name, ios::binary);
        ofstream ofs(json_file_name);
        const uint8_t addr[] = {0x72, 0x57, 0xc2, 0x41, 0x7e, 0x4d, 0x10, 0x38, 0xe1, 0x81, 0x7c, 0x8f, 0x28, 0x3a, 0xce, 0x2e};

        if (!mv.is_open())
            throw runtime_error(format("file %s is not exist", mv_file_name.c_str()));

        mv.seekg(0, mv.end);
        int length = mv.tellg();
        vector<uint8_t> buffer(length);

        mv.seekg(0, mv.beg);
        mv.read((char *)buffer.data(), buffer.size());

        auto pos = search(begin(buffer), end(buffer), addr, end(addr));
        if (pos != end(buffer))
            copy((char *)address.data(), (char *)end(address), pos);

        //
        //  generate the format likes {"code" : […], "args" : []}
        //
        ofs << R"({"code" : [)";

        transform(
            begin(buffer), end(buffer),
            ostream_iterator<string>(ofs),
            [](uint8_t num) -> auto {
                return to_string(num) + ",";
            });

        ofs.seekp(-1, ios_base::cur) << R"(], "args" : []})";
    }

    void replace_mv_with_addr(const std::string &mv_file_name,
                              const std::string &new_file_name,
                              const Address &address)
    {
        ifstream mv(mv_file_name, ios::binary);
        ofstream ofs(new_file_name);
        const uint8_t addr[] = {0x72, 0x57, 0xc2, 0x41, 0x7e, 0x4d, 0x10, 0x38, 0xe1, 0x81, 0x7c, 0x8f, 0x28, 0x3a, 0xce, 0x2e};

        if (!mv.is_open())
            throw runtime_error(format("file %s is not exist", mv_file_name.c_str()));

        mv.seekg(0, mv.end);
        int length = mv.tellg();
        vector<uint8_t> buffer(length);

        mv.seekg(0, mv.beg);
        mv.read((char *)buffer.data(), buffer.size());

        auto pos = begin(buffer);
        do
        {
            pos = search(pos, end(buffer), addr, end(addr));
            if (pos != end(buffer))
            {
                auto data = address.data();
                copy(begin(data), end(data), pos);
            }
        } while (pos != end(buffer));

        copy(begin(buffer), end(buffer), ostreambuf_iterator<char>(ofs));
    }

    Address::Address(const uint8_t *data, uint64_t len)
    {
        if (len > length)
            len = length;

        copy(data, data + len, m_data.data());
    }

    std::string Address::to_string() const
    {
        ostringstream oss;
        oss << m_data;

        return oss.str();
    }

    Address Address::from_string(const std::string &hex_addr)
    {
        Address address;
        auto data = bytes_from_string<length>(hex_addr);

        copy(begin(data), end(data), begin(address.m_data));

        return address;
    }

    std::ostream &operator<<(std::ostream &os, const Address &address)
    {
        os << address.m_data;

        return os;
    }

    // std::istream &operator>>(std::istream &is, Address & address)
    // {
    //     return is;
    // }

    bool Address::operator==(const Address &right) const
    {
        return m_data == right.m_data;
    }
    //
    //  ClientImp
    //  the implimentation of interface Clinet
    //
    class ClientImp : virtual public Client
    {
    protected:
        void *raw_client_proxy = nullptr;

        vector<pair<uint64_t, Address>> m_accounts;

    public:
        ClientImp(const std::string &url,
                  const std::string &mint_key_file_name,
                  bool sync_on_wallet_recovery,
                  const std::string &faucet_server,
                  const std::string &mnemonic_file_name,
                  const std::string &waypoint)
        {

            bool ret = (void *)violas_create_client(url.data(),
                                                    mint_key_file_name.data(),
                                                    sync_on_wallet_recovery,
                                                    faucet_server.data(),
                                                    mnemonic_file_name.data(),
                                                    waypoint.data(),
                                                    (uint64_t *)&raw_client_proxy);
            if (!ret)
                throw runtime_error(format("failed to create native Violas client, error : %s",
                                           get_last_error().c_str()));

            CLOG << "\ncreate violas client with "
                 << "\n\turl = " << url
                 << "\n\tmint_key = " << mint_key_file_name
                 << "\n\tsync_on_wallet_recovery = " << sync_on_wallet_recovery
                 << "\n\tfaucet_server = " << faucet_server
                 << "\n\tmnemonic_file = " << mnemonic_file_name
                 << endl;
        }

        virtual ~ClientImp()
        {
            libra_destory_client_proxy((uint64_t)raw_client_proxy);
            raw_client_proxy = 0;
        }

        string get_last_error()
        {
            char *last_error = libra_get_last_error();
            string error = last_error;
            libra_free_string(last_error);

            return error;
        }

        virtual void test_validator_connection() override
        {
            bool ret = libra_test_validator_connection((uint64_t)raw_client_proxy);
            if (!ret)
                throw runtime_error("failed to test validator connection");

            CLOG << "\nsucceeded to test validator connection" << endl;
        }

        virtual std::pair<size_t, Address>
        create_next_account(bool sync_with_validator) override
        {
            Libra_Address libra_account = libra_create_next_account((uint64_t)raw_client_proxy,
                                                                    sync_with_validator);

            auto account = make_pair<>(libra_account.index, Address(libra_account.address, ADDRESS_LENGTH));

            m_accounts.push_back(account);

            return account;
        }

        virtual std::vector<Account> get_all_accounts() override
        {
            Accounts all_accounts = libra_get_all_accounts((uint64_t)raw_client_proxy);

            vector<Account> accounts;

            for (uint64_t i = 0; i < all_accounts.len; i++)
            {
                Account a;
                const auto &_a = all_accounts.data[i];

                a.address = Address(_a.address, ADDRESS_LENGTH);
                a.index = _a.index;
                a.sequence_number = _a.sequence_number;
                a.status = _a.status;

                accounts.push_back(a);
            }

            libra_free_all_accounts_buf(all_accounts);

            return accounts;
        }

        virtual uint64_t get_balance(uint64_t account_index) override
        {
            const auto &address = m_accounts.at(account_index).second;

            return get_balance(address);
        }

        virtual uint64_t get_balance(const Address &address) override
        {
            uint64_t balance = 0.f;

            bool ret = libra_get_balance((uint64_t)raw_client_proxy,
                                         address.data().data(),
                                         &balance);
            if (!ret)
                throw runtime_error(
                    format("failed to get balance, error : %s", get_last_error().c_str()));

            return balance;
        }

        virtual uint64_t get_sequence_number(uint64_t account_index) override
        {
            auto [index, address] = m_accounts.at(account_index);

            return get_sequence_number(address);
        }

        virtual uint64_t get_sequence_number(const Address &address) override
        {
            uint64_t sequence_num = 0;

            bool ret = libra_get_sequence_number((uint64_t)raw_client_proxy,
                                                 address.data().data(),
                                                 sequence_num);
            if (!ret)
                throw runtime_error(
                    format("failed to get sequence number, error : %s", get_last_error().c_str()));

            return sequence_num;
        }

        virtual void mint_coins(uint64_t index,
                                uint64_t num_coins,
                                bool is_blocking) override
        {
            bool ret = libra_mint_coins((uint64_t)raw_client_proxy, index, num_coins, is_blocking);
            if (!ret)
            {
                throw runtime_error(
                    format("failed to mint coins, error : %s", get_last_error().c_str()));
            }
        }

        virtual std::pair<uint64_t, uint64_t>
        transfer_coins_int(uint64_t sender_account_ref_id,
                           Address receiver,
                           uint64_t num_coins,
                           uint64_t gas_unit_price = 0,
                           uint64_t max_gas_amount = 0,
                           bool is_blocking = true) override
        {
            _index_sequence index_seq;
            bool ret = libra_transfer_coins_int(
                (uint64_t)raw_client_proxy, sender_account_ref_id,
                (uint8_t *)receiver.data().data(),
                num_coins, gas_unit_price, max_gas_amount,
                is_blocking, &index_seq);
            if (!ret)
                throw runtime_error(
                    format("failed to transfer coins, error : %s", get_last_error().c_str()));

            return make_pair(index_seq.index, index_seq.sequence);
        }

        virtual void compile(Address account_address,
                             const std::string &source_file,
                             bool is_module,
                             const std::string &temp_dir) override
        {
            auto addr = account_address.to_string();
            bool ret = libra_compile((uint64_t)raw_client_proxy, addr.c_str(),
                                     source_file.c_str(), is_module, temp_dir.c_str());
            if (!ret)
            {

                throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                           source_file.c_str(),
                                           get_last_error().c_str()));
            }

            CLOG << "compiled '" << source_file << "', "
                 << "is_module = " << (is_module ? "true" : "false") << endl;
        }

        virtual void compile(uint64_t account_index,
                             const string &source_file_with_path,
                             bool is_module,
                             const std::string &temp_dir) override
        {
            bool ret = libra_compile((uint64_t)raw_client_proxy,
                                     to_string(account_index).c_str(),
                                     source_file_with_path.c_str(), is_module, temp_dir.c_str());
            if (!ret)
            {

                throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                           source_file_with_path.c_str(),
                                           get_last_error().c_str()));
            }

            CLOG << "compiled '" << source_file_with_path << "', "
                 << "is_module = " << (is_module ? "true" : "false") << endl;
        }

        virtual void publish_module(uint64_t account_index,
                                    const std::string &module_file) override
        {
            bool ret = libra_publish_module((uint64_t)raw_client_proxy, account_index,
                                            module_file.c_str());
            if (!ret)
            {
                auto error = format("failed to publish module file '%s', error : %s",
                                    module_file.c_str(), get_last_error().c_str());
                throw runtime_error(error);
            }

            CLOG << "published module " << module_file << endl;
        }

        virtual void publish_module_with_faucet_account(const std::string &module_file) override
        {
            bool ret = violas_publish_module_with_association_account((uint64_t)raw_client_proxy,
                                                                module_file.c_str());
            if (!ret)
            {
                auto error = format("failed to publish module file '%s' with faucet account, error : %s",
                                    module_file.c_str(), get_last_error().c_str());
                throw runtime_error(error);
            }

            CLOG << "published module '" << module_file << "' with faucet account" << endl;
        }

        virtual void
        execute_script(uint64_t account_index, const std::string &script_file,
                       const std::vector<std::string> &script_args) override
        {
            ScriptArgs args;

            vector<const char *> args_array;
            for (auto &arg : script_args)
            {
                args_array.push_back(arg.c_str());
            }

            args.len = script_args.size();
            args.data = args_array.data();

            bool ret = libra_execute_script((uint64_t)raw_client_proxy, account_index,
                                            script_file.c_str(), &args);
            if (!ret)
                throw runtime_error(
                    format("failed to execute script file '%s' for account index %d, "
                           "error : %s, "
                           "at %s",
                           script_file.c_str(),
                           account_index,
                           get_last_error().c_str(),
                           EXCEPTION_AT.c_str()));

            CLOG << format("excuted script file '%s' for account index %d",
                           script_file.c_str(), account_index)
                 << endl;
        }

        /// execute script with faucet account
        virtual void execute_script_with_faucet_account(const std::string &script_file,
                                                        const std::vector<std::string> &script_args) override
        {
            ScriptArgs args;

            vector<const char *> args_array;
            for (auto &arg : script_args)
            {
                args_array.push_back(arg.c_str());
            }

            args.len = script_args.size();
            args.data = args_array.data();

            bool ret = violas_execute_script_with_association_account((uint64_t)raw_client_proxy,
                                                                script_file.c_str(),
                                                                &args);
            if (!ret)
                throw runtime_error(
                    format("failed to execute script file '%s' for account faucet, "
                           "error : %s, "
                           "at %s",
                           script_file.c_str(),
                           get_last_error().c_str(),
                           EXCEPTION_AT.c_str()));

            CLOG << format("excuted script file '%s' for account faucet", script_file.c_str())
                 << endl;
        }
        virtual std::pair<std::string, std::string>
        get_committed_txn_by_acc_seq(uint64_t account_index,
                                     uint64_t sequence_num) override
        {
            const auto &address = m_accounts.at(account_index).second;

            return get_committed_txn_by_acc_seq(address, sequence_num);
        }

        virtual std::pair<std::string, std::string>
        get_committed_txn_by_acc_seq(Address address, uint64_t sequence_num) override
        {
            char *out_txn = nullptr, *events = nullptr;

            bool ret = libra_get_committed_txn_by_acc_seq((uint64_t)raw_client_proxy,
                                                          address.data().data(),
                                                          sequence_num,
                                                          &out_txn, &events);
            if (!ret)
                throw runtime_error(format("failed to get committed transaction by "
                                           "account index %d and sequence number %d, "
                                           "error : %s",
                                           address.to_string().c_str(),
                                           sequence_num,
                                           get_last_error().c_str()) +
                                    EXCEPTION_AT);

            CLOG << format("get committed transaction by account index %d and sequence number %d",
                           address.to_string().c_str(), sequence_num)
                 << endl;

            auto txn_events = make_pair<string, string>(out_txn, events);

            libra_free_string(out_txn);
            libra_free_string(events);

            return txn_events;
        }

        virtual std::vector<std::pair<std::string, std::string>>
        get_txn_by_range(uint64_t start_version, uint64_t limit, bool fetch_events) override
        {
            std::vector<std::pair<std::string, std::string>> vec_txn_events;

            AllTxnEvents all_txn_events{nullptr, 0, 0};

            bool ret = libra_get_txn_by_range((uint64_t)raw_client_proxy, start_version, limit,
                                              fetch_events, &all_txn_events);
            if (!ret)
            {
                throw runtime_error(
                    format("failed to get tansaction and events by range, %d ",
                           get_last_error().c_str()) +
                    EXCEPTION_AT);
            }

            for (uint64_t i = 0; i < all_txn_events.len; i++)
            {
                auto txn_events = make_pair(all_txn_events.data[i].transaction,
                                            all_txn_events.data[i].events);
                vec_txn_events.push_back(txn_events);
            }

            libra_free_all_txn_events(&all_txn_events);

            return vec_txn_events;
        }
        //
        /// Get events by account index and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(uint64_t account_index,
                   EventType type,
                   uint64_t start_seq_number,
                   uint64_t limit) override
        {
            const auto &address = m_accounts.at(account_index).second;
            return get_events(address, type, start_seq_number, limit);
        }
        //
        // Get events by account and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(Address address,
                   EventType type,
                   uint64_t start_seq_number,
                   uint64_t limit) override
        {
            StrArrray out_all_events = {nullptr, 0, 0};
            char *out_last_account_event;

            bool ret = libra_get_events((uint64_t)raw_client_proxy,
                                        address.data().data(),
                                        (libra_event_type)type,
                                        start_seq_number, limit,
                                        &out_all_events,
                                        &out_last_account_event);
            if (!ret)
            {
                throw runtime_error(format("failed to get events, errror : %d ",
                                           get_last_error().c_str()));
            }

            vector<string> all_events;
            for (uint64_t i = 0; i < out_all_events.len; i++)
            {
                all_events.push_back(out_all_events.data[i]);
            }
            violas_free_str_array(&out_all_events);

            string last_account_event = out_last_account_event;
            libra_free_string(out_last_account_event);

            return make_pair<>(all_events, last_account_event);
        }

        virtual uint64_t get_account_resource_uint64(uint64_t account_index,
                                                     const Address &res_path_addr,
                                                     uint64_t token_index) override
        {
            uint64_t result = 0;
            string path_addr = "0x" + res_path_addr.to_string();

            bool ret = libra_get_account_resource(
                (uint64_t)raw_client_proxy, to_string(account_index).c_str(),
                path_addr.c_str(), token_index, &result);
            if (!ret)
                throw runtime_error(
                    format("failed to get get resource for account index %d ", account_index) +
                    EXCEPTION_AT);

            return result;
        }

        virtual uint64_t get_account_resource_uint64(const Address &account_addr,
                                                     const Address &res_path_addr,
                                                     uint64_t token_index) override
        {
            uint64_t result = 0;
            string path_addr = "0x" + res_path_addr.to_string();
            auto addr = account_addr.to_string();

            bool ret = libra_get_account_resource(
                (uint64_t)raw_client_proxy, addr.c_str(), path_addr.c_str(), token_index, &result);
            if (!ret)
                throw runtime_error(
                    format("failed to get get resource for account address %s ", addr.c_str()) +
                    EXCEPTION_AT);

            return result;
        }
    };

    std::shared_ptr<Client>
    Client::create(const std::string &url,
                   const std::string &mint_key_file_name,
                   bool sync_on_wallet_recovery,
                   const std::string &faucet_server,
                   const std::string &mnemonic_file_name,
                   const std::string &waypoint)
    {
        return make_shared<ClientImp>(url,
                                      mint_key_file_name,
                                      sync_on_wallet_recovery,
                                      faucet_server,
                                      mnemonic_file_name,
                                      waypoint);
    }

    class TokenManagerImp : public TokenManager
    {
    public:
        TokenManagerImp(client_ptr client,
                        Address governor_addr,
                        const std::string &name,
                        const std::string &script_files_path)
            : m_libra_client(client),
              m_name(name),
              m_supervisor(governor_addr)
        {
            init_all_script(script_files_path);
        }

        TokenManagerImp(client_ptr client,
                        Address governor_addr,
                        const std::string &name,
                        function<void(const std::string &)> init_all_script_fun,
                        const std::string &temp_path)
            : m_libra_client(client),
              m_name(name),
              m_supervisor(governor_addr),
              m_temp_path(temp_path)
        {
            string governor = m_supervisor.to_string();

            m_temp_script_path = m_temp_path / m_supervisor.to_string();

            fs::remove_all(m_temp_script_path);

            fs::create_directory(m_temp_script_path);
            CLOG << m_temp_script_path.string() << endl;

            init_all_script_fun(m_temp_script_path.string());
        }

        virtual ~TokenManagerImp() {}

        virtual std::string name() override { return m_name; }

        virtual Address address() override { return m_supervisor; }

        virtual void deploy(uint64_t account_index) override
        {
            // make sure the account-index has the deploying permission
            // assert(m_libra_client->get_all_accounts()[account_index].address == m_supervisor);

            auto script_file_name = m_temp_script_path / token_module;

            try_compile(script_file_name, true);

            m_libra_client->publish_module(account_index, (script_file_name += ".mv").c_str());
        }

        virtual void publish(uint64_t account_index, const std::string &user_data) override
        {
            auto script_file_name = m_temp_script_path / publish_script;

            try_compile(script_file_name);

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(),
                                           vector<string>{tx_vec_data(user_data)});
        }

        virtual void create_token(uint64_t account_index, Address owner, const std::string &token_data = "token") override
        {
            auto script_file_name = m_temp_script_path / create_token_script;

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(),
                                           vector<string>{owner.to_string(), tx_vec_data(token_data)});
        }

        virtual void mint(uint64_t token_index,
                          uint64_t account_index,
                          Address receiver,
                          uint64_t amount_micro_coin,
                          const std::string &data) override
        {
            // make sure the account-index has the minting permission
            //assert(m_libra_client->get_all_accounts()[account_index].address == m_supervisor);

            auto script_file_name = m_temp_script_path / mint_script;

            try_compile(script_file_name);

            auto args = vector<string>{to_string(token_index),
                                       receiver.to_string(),
                                       to_string(amount_micro_coin),
                                       tx_vec_data(data)};

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(), args);
        }

        virtual void transfer(uint64_t token_index,
                              uint64_t account_index,
                              Address receiver,
                              uint64_t amount_micro_coin,
                              const std::string &data) override
        {
            auto script_file_name = m_temp_script_path / transfer_script;

            try_compile(script_file_name);

            auto args = vector<string>{to_string(token_index),
                                       receiver.to_string(),
                                       to_string(amount_micro_coin),
                                       tx_vec_data(data)};

            m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(), args);
        }

        virtual uint64_t token_count() override
        {
            return 0;
        }

        virtual uint64_t get_account_balance(uint64_t token_index, uint64_t account_index) override
        {
            uint64_t balance = m_libra_client->get_account_resource_uint64(account_index,
                                                                           m_supervisor,
                                                                           token_index);
            return balance;
        }

        virtual uint64_t get_account_balance(uint64_t token_index, Address account_address) override
        {
            uint64_t balance = m_libra_client->get_account_resource_uint64(account_address,
                                                                           m_supervisor,
                                                                           token_index);
            return balance;
        }

    protected:
        void init_all_script(const string &script_files_path)
        {
            string governor = m_supervisor.to_string();

            if (m_temp_path.empty())
                m_temp_path = fs::temp_directory_path(); // /tmp/xxxxx

            m_temp_script_path = m_temp_path / fs::path(governor);

            fs::remove_all(m_temp_script_path);

            fs::create_directory(m_temp_script_path);
            CLOG << m_temp_script_path.string() << endl;

            for (auto &file : fs::directory_iterator(fs::path(script_files_path)))
            {
                if (file.path().extension() == ".mv")
                {
                    auto new_file = m_temp_script_path / file.path().filename().string();

#if __cplusplus >= 201703L
                    //auto option = fs::copy_options::overwrite_existing;
#else
                    //auto option = fs::copy_option::overwrite_if_exists;
#endif
                    //fs::copy_file(file.path().string(), new_file, option);

                    replace_mv_with_addr(file.path().string(), new_file, m_supervisor);
                }
            }
        }

        void try_compile(fs::path script_file_name, bool is_module = false)
        {
            //auto mv(fs::path(script_file_name) += ".mv");
            //auto mvir(fs::path(script_file_name) += ".mvir");

            //if (!fs::exists(mv))
            //    m_libra_client->compile(m_supervisor, mvir.c_str(), is_module, m_temp_path.string());
        }

    private:
        client_ptr m_libra_client;
        string m_name;
        Address m_supervisor;
        string m_module;
        fs::path m_temp_path;
        fs::path m_temp_script_path;
        const string token_module = "token";
        const string publish_script = "publish";
        const string create_token_script = "create_token";
        const string mint_script = "mint";
        const string transfer_script = "transfer";
    };

    std::shared_ptr<TokenManager> TokenManager::create(client_ptr client,
                                                       Address governor_addr,
                                                       const std::string &name,
                                                       const std::string &script_files_path)
    {
        return make_shared<TokenManagerImp>(client, governor_addr, name, script_files_path);
    }

    std::shared_ptr<TokenManager> TokenManager::create(client_ptr client,
                                                       Address governor_addr,
                                                       const std::string &name,
                                                       function<void(const std::string &)> init_all_script_fun,
                                                       const std::string &temp_path)
    {
        return make_shared<TokenManagerImp>(client, governor_addr, name, init_all_script_fun, temp_path);
    }

} // namespace LIB_NAME

#ifdef PYTHON
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ClientImp_transfer_overloads, ClientImp::transfer_coins_int, 3, 6)

BOOST_PYTHON_MODULE(violas)
{
    using namespace Violas;

    class_<std::pair<ulong, uint256>>("IntPair")
        .def_readwrite("first", &std::pair<ulong, uint256>::first)
        .def_readwrite("second", &std::pair<ulong, uint256>::second);

    class_<std::array<unsigned char, 32ul>>("uint256");
    class_<std::pair<ulong, ulong>>("UlongPair");

    def("uint256_to_string", uint256_to_string);
    def("uint256_from_string", uint256_from_string);

    class_<Client::Account>("Account")
        .add_property("index", &Client::Account::index)
        .add_property("address", &Client::Account::address)
        .add_property("sequence_number", &Client::Account::sequence_number)
        .add_property("status", &Client::Account::status);

    using Accounts = std::vector<Client::Account>;
    class_<Accounts>("Accounts")
        .def(vector_indexing_suite<Accounts>());

    void (ClientImp::*compile1)(uint64_t, const string &, bool) = &ClientImp::compile;

    class_<ClientImp, std::shared_ptr<ClientImp>>("Client", init<string, uint16_t, string, string, bool, string, string>())
        .def("test_validator_connection", &ClientImp::test_validator_connection)
        .def("create_next_account", &ClientImp::create_next_account)
        .def("get_all_accounts", &ClientImp::get_all_accounts)
        .def("get_balance", (double (ClientImp::*)(uint64_t index)) & ClientImp::get_balance)
        .def("get_sequence_number", &ClientImp::get_sequence_number)
        .def("mint_coins", &ClientImp::mint_coins)
        .def("transfer", &ClientImp::transfer_coins_int, ClientImp_transfer_overloads())
        .def("compile", compile1)
        .def("publish_module", &ClientImp::publish_module)
        .def("execute_script", &ClientImp::execute_script)
        .def("get_committed_txn_by_acc_seq", &ClientImp::get_committed_txn_by_acc_seq)
        .def("get_txn_by_range", &ClientImp::get_txn_by_range);

    uint64_t (TokenImp::*get_account_balance1)(uint64_t) = &TokenImp::get_account_balance;
    uint64_t (TokenImp::*get_account_balance2)(uint256) = &TokenImp::get_account_balance;

    class_<TokenImp, std::shared_ptr<TokenImp>>("Token", init<client_ptr, uint256, string, string>())
        .def("name", &TokenImp::name)
        .def("address", &TokenImp::address)
        .def("deploy", &TokenImp::deploy)
        .def("mint", &TokenImp::mint)
        .def("publish", &TokenImp::publish)
        .def("transfer", &TokenImp::transfer)
        .def("get_account_balance", get_account_balance1)
        .def("get_account_balance", get_account_balance2);

    implicitly_convertible<std::shared_ptr<ClientImp>, std::shared_ptr<Client>>();
}

#endif
