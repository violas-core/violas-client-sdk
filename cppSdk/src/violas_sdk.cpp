#include <ctime>
#include <iomanip>
//#include <chrono>
#include <cassert>
#include <sstream>
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

namespace Violas
{

ostream &operator<<(ostream &os, const uint256 &value)
{
    for (auto v : value)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    os << std::dec;

    return os;
}

std::ostream &operator>>(std::ostream &os, const uint256 &value) { return os; }

std::string uint256_to_string(const uint256 &address)
{
    ostringstream oss;
    oss << address;

    return oss.str();
}

uint256 uint256_from_string(const std::string &str)
{
    uint256 addr;
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

bool is_valid_balance(uint64_t value)
{
#if __cplusplus >= 201703L
    const uint64_t max_uint64 = numeric_limits<uint64_t>::max();
#else
    const uint64_t max_uint64 = 0xFFFFFFFFFFFFFFFF;
#endif
    return value != max_uint64;
}

class ClientImp : virtual public Client
{
protected:
    void *raw_client_proxy = nullptr;

public:
    ClientImp(const std::string &host,
              uint16_t port,
              const std::string &validator_set_file,
              const std::string &faucet_account_file,
              bool sync_on_wallet_recovery,
              const std::string &faucet_server,
              const std::string &mnemonic_file)
    {
        raw_client_proxy = (void *)libra_create_client_proxy(
            host.data(), port, validator_set_file.data(),
            faucet_account_file.data(), sync_on_wallet_recovery,
            faucet_server.data(), mnemonic_file.data());
        if (raw_client_proxy == nullptr)
            throw runtime_error(format("failed to create native rust client proxy, error : %s",
                                       get_last_error().c_str()));

        CLOG << "\ncreate violas client with "
             << "\n\thost = " << host
             << "\n\tport = " << port
             << "\n\tvalidator_set_file = " << validator_set_file
             << "\n\tfaucet_account_file = " << faucet_account_file
             << "\n\tsync_on_wallet_recovery = " << sync_on_wallet_recovery
             << "\n\tfaucet_server = " << faucet_server
             << "\n\tmnemonic_file = " << mnemonic_file
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

    virtual std::pair<size_t, uint256>
    create_next_account(bool sync_with_validator) override
    {
        Address account = libra_create_next_account((uint64_t)raw_client_proxy,
                                                    sync_with_validator);

        uint256 address;
        copy(begin(account.address), end(account.address), begin(address));

        return make_pair<>(account.index, address);
    }

    virtual std::vector<Account> get_all_accounts() override
    {
        Accounts all_accounts = libra_get_all_accounts((uint64_t)raw_client_proxy);

        vector<Account> accounts;

        for (uint64_t i = 0; i < all_accounts.len; i++)
        {
            Account a;
            const auto &_a = all_accounts.data[i];

            copy(begin(_a.address), end(_a.address), begin(a.address));
            a.index = _a.index;
            a.sequence_number = _a.sequence_number;
            a.status = _a.status;

            accounts.push_back(a);
        }

        libra_free_all_accounts_buf(all_accounts);

        return accounts;
    }

    virtual double get_balance(uint64_t index) override
    {
        double balance = 0.f;

        bool ret = libra_get_balance((uint64_t)raw_client_proxy, to_string(index).c_str(),
                                     &balance);
        if (!ret)
            throw runtime_error(
                format("failed to get balance, error : %s", get_last_error().c_str()));

        return balance;
    }

    virtual double get_balance(uint256 address) override
    {
        double balance = 0.f;

        bool ret = libra_get_balance((uint64_t)raw_client_proxy,
                                     uint256_to_string(address).c_str(), &balance);
        if (!ret)
            throw runtime_error(
                format("failed to get balance, error : %s", get_last_error().c_str()));

        return balance;
    }

    virtual uint64_t get_sequence_number(uint64_t index) override
    {
        return libra_get_sequence_number((uint64_t)raw_client_proxy, index);
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
                       uint256 receiver_address,
                       uint64_t num_coins,
                       uint64_t gas_unit_price = 0,
                       uint64_t max_gas_amount = 0,
                       bool is_blocking = true) override
    {
        _index_sequence index_seq;
        bool ret = libra_transfer_coins_int(
            (uint64_t)raw_client_proxy, sender_account_ref_id,
            receiver_address.data(), num_coins, gas_unit_price, max_gas_amount,
            is_blocking, &index_seq);
        if (!ret)
            throw runtime_error(
                format("failed to transfer coins, error : %s", get_last_error().c_str()));

        return make_pair(index_seq.index, index_seq.sequence);
    }

    virtual void compile(uint256 account_address,
                         const std::string &source_file,
                         bool is_module,
                         const std::string &temp_dir) override
    {
        auto addr = uint256_to_string(account_address);
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

    virtual std::pair<std::string, std::string>
    get_committed_txn_by_acc_seq(uint64_t account_index,
                                 uint64_t sequence_num) override
    {
        char *out_txn = nullptr, *events = nullptr;

        bool ret = libra_get_committed_txn_by_acc_seq((uint64_t)raw_client_proxy,
                                                      account_index, sequence_num,
                                                      &out_txn, &events);
        if (!ret)
            throw runtime_error(format("failed to get committed transaction by "
                                       "account index %d and sequence number %d, "
                                       "error : %s",
                                       account_index, sequence_num,
                                       get_last_error().c_str()) +
                                EXCEPTION_AT);

        CLOG << format("get committed transaction by account index %d and sequence number %d",
                       account_index, sequence_num)
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

    virtual uint64_t
    get_account_resource_uint64(uint64_t account_index, const uint256 &res_path_addr) override
    {
        uint64_t result = 0;
        string path_addr = "0x" + uint256_to_string(res_path_addr);

        bool ret = libra_get_account_resource(
            (uint64_t)raw_client_proxy, to_string(account_index).c_str(),
            path_addr.c_str(), &result);
        if (!ret)
            throw runtime_error(
                format("failed to get get resource for account index %d ", account_index) +
                EXCEPTION_AT);

        return result;
    }

    virtual uint64_t get_account_resource_uint64(const uint256 &account_addr,
                                                 const uint256 &res_path_addr) override
    {
        uint64_t result = 0;
        string path_addr = "0x" + uint256_to_string(res_path_addr);
        auto addr = uint256_to_string(account_addr);

        bool ret = libra_get_account_resource(
            (uint64_t)raw_client_proxy, addr.c_str(), path_addr.c_str(), &result);
        if (!ret)
            throw runtime_error(
                format("failed to get get resource for account address %s ", addr.c_str()) +
                EXCEPTION_AT);

        return result;
    }
};

std::shared_ptr<Client> Client::create(const std::string &host,
                                       uint16_t port,
                                       const std::string &validator_set_file,
                                       const std::string &faucet_account_file,
                                       bool sync_on_wallet_recovery,
                                       const std::string &faucet_server,
                                       const std::string &mnemonic_file)
{
    return make_shared<ClientImp>(host, port, validator_set_file,
                                  faucet_account_file, sync_on_wallet_recovery,
                                  faucet_server, mnemonic_file);
}

class TokenImp : public Token
{
public:
    TokenImp(client_ptr client,
             uint256 governor_addr,
             const std::string &name,
             const std::string &script_files_path)
        : m_libra_client(client),
          m_name(name),
          m_governor_addr(governor_addr)
    {
        init_all_script(script_files_path);
    }

    TokenImp(client_ptr client,
             uint256 governor_addr,
             const std::string &name,
             function<void(const std::string &)> init_all_script_fun,
             const std::string &temp_path)
        : m_libra_client(client),
          m_name(name),
          m_governor_addr(governor_addr),
          m_temp_path(temp_path)
    {
        string governor = uint256_to_string(m_governor_addr);

        m_temp_script_path = m_temp_path / uint256_to_string(m_governor_addr);

        fs::remove_all(m_temp_script_path);

        fs::create_directory(m_temp_script_path);
        CLOG << m_temp_script_path.string() << endl;

        init_all_script_fun(m_temp_script_path.string());
    }

    virtual ~TokenImp() {}

    virtual std::string name() override { return m_name; }

    virtual uint256 address() override { return m_governor_addr; }

    virtual void deploy(uint64_t account_index) override
    {
        // make sure the account-index has the deploying permission
        // assert(m_libra_client->get_all_accounts()[account_index].address == m_governor_addr);

        auto script_file_name = m_temp_script_path / token_module;

        try_compile(script_file_name, true);

        m_libra_client->publish_module(account_index, (script_file_name += ".mv").c_str());
    }

    virtual void mint(uint64_t account_index, uint256 address, uint64_t amount) override
    {
        // make sure the account-index has the minting permission
        assert(m_libra_client->get_all_accounts()[account_index].address == m_governor_addr);

        auto script_file_name = m_temp_script_path / mint_script;

        try_compile(script_file_name);

        auto args = vector<string>{uint256_to_string(address), to_string(amount)};

        m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(), args);
    }

    virtual void publish(uint64_t account_index) override
    {
        auto script_file_name = m_temp_script_path / publish_script;

        try_compile(script_file_name);

        m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(),
                                       vector<string>{});
    }

    virtual void
    transfer(uint64_t account_index, uint256 address, uint64_t amount_micro_coin) override
    {
        auto script_file_name = m_temp_script_path / publish_script;

        try_compile(script_file_name);

        auto args =
            vector<string>{uint256_to_string(address), to_string(amount_micro_coin)};

        m_libra_client->execute_script(account_index, (script_file_name += ".mv").c_str(), args);
    }

    virtual uint64_t get_account_balance(uint64_t account_index) override
    {
        uint64_t balance = m_libra_client->get_account_resource_uint64(account_index,
                                                                       m_governor_addr);
        return balance;
    }

    virtual uint64_t get_account_balance(uint256 account_addr) override
    {
        uint64_t balance = m_libra_client->get_account_resource_uint64(account_addr,
                                                                       m_governor_addr);
        return balance;
    }

protected:
    void init_all_script(const string &script_files_path)
    {
        string governor = uint256_to_string(m_governor_addr);

        if (m_temp_path.empty())
            m_temp_path = fs::temp_directory_path(); // /tmp/xxxxx

        m_temp_script_path = m_temp_path / fs::path(governor);

        fs::remove_all(m_temp_script_path);

        fs::create_directory(m_temp_script_path);
        CLOG << m_temp_script_path.string() << endl;

        for (auto &file : fs::directory_iterator(fs::path(script_files_path)))
        {
            if (file.path().extension() == ".mvir")
            {
                auto new_file = m_temp_script_path / file.path().filename().string();

#if __cplusplus >= 201703L
                auto option = fs::copy_options::overwrite_existing;
#else
                auto option = fs::copy_option::overwrite_if_exists;
#endif
                fs::copy_file(file.path().string(), new_file, option);
            }
        }
    }

    void try_compile(fs::path script_file_name, bool is_module = false)
    {
        auto mv(fs::path(script_file_name) += ".mv");
        auto mvir(fs::path(script_file_name) += ".mvir");

        if (!fs::exists(mv))
            m_libra_client->compile(m_governor_addr, mvir.c_str(), is_module, m_temp_path.string());
    }

private:
    client_ptr m_libra_client;
    string m_name;
    uint256 m_governor_addr;
    string m_module;
    fs::path m_temp_path;
    fs::path m_temp_script_path;
    const string token_module = "token";
    const string publish_script = "publish";
    const string mint_script = "mint";
    const string transfer_script = "transfer";
};

std::shared_ptr<Token> Token::create(client_ptr client,
                                     uint256 governor_addr,
                                     const std::string &name,
                                     const std::string &script_files_path)
{
    return make_shared<TokenImp>(client, governor_addr, name, script_files_path);
}

std::shared_ptr<Token> Token::create(client_ptr client,
                                     uint256 governor_addr,
                                     const std::string &name,
                                     function<void(const std::string &)> init_all_script_fun,
                                     const std::string &temp_path)
{
    return make_shared<TokenImp>(client, governor_addr, name, init_all_script_fun, temp_path);
}

} // namespace Violas

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
