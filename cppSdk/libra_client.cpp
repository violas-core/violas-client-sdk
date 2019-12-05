#include <ctime>
#include <iomanip>
//#include <chrono>
#include <cassert>
#include <sstream>
#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#include "libra_client.hpp"
#include "rust_client_proxy.hpp"

using namespace std;

inline ostream &log(ostream &ost, const char *flag, const char *file, int line,
                    const char *func)
{
    time_t now = time(nullptr);

    ost << flag << put_time(std::localtime(&now), "%F %T") << " (" << file << ":"
        << line << ":" << func << ") : ";

    return ost;
}

#define EXCEPTION_AT \
    format(", exception at (%s:%s:%d)", __FILE__, __func__, __LINE__)

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

bool is_valid_balance(uint64_t value)
{
#if __cplusplus >= 201703L
    const uint64_t max_uint64 = numeric_limits<uint64_t>::max();
#else
    const uint64_t max_uint64 = 0xFFFFFFFFFFFFFFFF;
#endif
    return value != max_uint64;
}

namespace Libra
{
class client_imp : virtual public client
{
protected:
    void *raw_client_proxy = nullptr;

public:
    client_imp(const std::string &host, uint16_t port,
               const std::string &validator_set_file,
               const std::string &faucet_account_file,
               bool sync_on_wallet_recovery, const std::string &faucet_server,
               const std::string &mnemonic_file)
    {
        raw_client_proxy = (void *)libra_create_client_proxy(
            host.data(), port, validator_set_file.data(),
            faucet_account_file.data(), sync_on_wallet_recovery,
            faucet_server.data(), mnemonic_file.data());
        if (raw_client_proxy == nullptr)
            throw runtime_error("failed to create native rust client proxy");
    }

    virtual ~client_imp()
    {
        libra_destory_client_proxy((uint64_t)raw_client_proxy);

        // LOG << "entered" << endl;
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

        bool ret = libra_get_balance((uint64_t)raw_client_proxy, index, &balance);
        if (!ret)
            throw runtime_error("failed to get balance ");

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
        libra_mint_coins((uint64_t)raw_client_proxy, index, num_coins, is_blocking);
    }

    virtual std::pair<uint64_t, uint64_t>
    transfer_coins_int(uint64_t sender_account_ref_id,
                       uint256 receiver_address,
                       uint64_t num_coins,
                       uint64_t gas_unit_price,
                       uint64_t max_gas_amount,
                       bool is_blocking) override
    {
        _index_sequence index_seq;
        bool ret = libra_transfer_coins_int(
            (uint64_t)raw_client_proxy, sender_account_ref_id,
            receiver_address.data(), num_coins, gas_unit_price, max_gas_amount,
            is_blocking, &index_seq);
        if (!ret)
            throw runtime_error("failed to transfer coins");

        return make_pair(index_seq.index, index_seq.sequence);
    }

    virtual void compile(uint256 account_address, const std::string &source_file, bool is_module = false) override
    {
        auto addr = uint256_to_string(account_address);
        bool ret = libra_compile((uint64_t)raw_client_proxy, addr.c_str(),
                                 source_file.c_str(), is_module);
        if (!ret)
        {

            throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                       source_file.c_str(),
                                       get_last_error().c_str()));
        }
        LOG << "compiled '" << source_file << "', "
            << "is_module = " << (is_module ? "true" : "false") << endl;
    }

    virtual void compile(uint64_t account_index,
                         const string &source_file_with_path,
                         bool is_module) override
    {
        bool ret = libra_compile((uint64_t)raw_client_proxy, to_string(account_index).c_str(),
                                 source_file_with_path.c_str(), is_module);
        if (!ret)
        {

            throw runtime_error(format("failed to compile move script file '%s', error : %s",
                                       source_file_with_path.c_str(),
                                       get_last_error().c_str()));
        }
        LOG << "compiled '" << source_file_with_path << "', "
            << "is_module = " << (is_module ? "true" : "false") << endl;
    }

    virtual void publish_module(uint64_t account_index,
                                const std::string &module_file) override
    {
        bool ret = libra_publish_module((uint64_t)raw_client_proxy, account_index,
                                        module_file.c_str());
        if (!ret)
        {
            auto error = format("failed to publish module file '%s', error : %s", module_file.c_str(), get_last_error().c_str());
            throw runtime_error(error);
        }

        LOG << "published module " << module_file << endl;
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
                format("failed to execute script file '%s' for account index %d",
                       script_file.c_str(), account_index) +
                EXCEPTION_AT);

        LOG << format("excuted script file '%s' for account index %d",
                      script_file.c_str(), account_index)
            << endl;
    }

    virtual std::pair<std::string, std::string> get_committed_txn_by_acc_seq(uint64_t account_index,
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

        LOG << format("get committed transaction by account index %d and sequence number %d", account_index, sequence_num)
            << endl;

        auto txn_events = make_pair<string, string>(out_txn, events);

        libra_free_string(out_txn);
        libra_free_string(events);

        return txn_events;
    }

    virtual uint64_t get_account_resource_uint64(uint64_t account_index, const uint256 &res_path_addr) override
    {
        uint64_t result = 0;
        string addr = "0x" + uint256_to_string(res_path_addr);

        bool ret = libra_get_account_resource(
            (uint64_t)raw_client_proxy, account_index, addr.c_str(), &result);
        if (!ret)
            throw runtime_error(
                format("failed to get get resource for account index %d ", account_index) + EXCEPTION_AT);

        return result;
    }
};

std::shared_ptr<client> client::create(const std::string &host,
                                       uint16_t port,
                                       const std::string &validator_set_file,
                                       const std::string &faucet_account_file,
                                       bool sync_on_wallet_recovery,
                                       const std::string &faucet_server,
                                       const std::string &mnemonic_file)
{
    return make_shared<client_imp>(host, port, validator_set_file,
                                   faucet_account_file, sync_on_wallet_recovery,
                                   faucet_server, mnemonic_file);
}

} // namespace Libra

namespace Violas
{
class TokenImp : public Token
{
public:
    TokenImp(Libra::client_ptr client,
             const std::string &name,
             uint256 governor_addr) : m_libra_client(client),
                                      m_name(name),
                                      m_governor_addr(governor_addr)
    {
        init_all_script();
    }

    virtual ~TokenImp() {}

    virtual std::string name() override { return m_name; }

    virtual uint256 address() override { return m_governor_addr; }

    virtual void deploy(uint64_t account_index) override
    {
        // make sure the account-index has the deploying permission
        // assert(m_libra_client->get_all_accounts()[account_index].address == m_governor_addr);

        auto script_name = m_temp_path / token_module;

        try_compile(script_name, true);

        m_libra_client->publish_module(account_index, (script_name += ".mv").c_str());
    }

    virtual void mint(uint64_t account_index, uint256 address, uint64_t amount) override
    {
        // make sure the account-index has the minting permission
        assert(m_libra_client->get_all_accounts()[account_index].address == m_governor_addr);

        auto script_name = m_temp_path / mint_script;

        try_compile(script_name);

        auto args = vector<string>{uint256_to_string(address), to_string(amount)};

        m_libra_client->execute_script(account_index, (script_name += ".mv").c_str(), args);
    }

    virtual void publish(uint64_t account_index) override
    {
        auto script_name = m_temp_path / publish_script;

        try_compile(script_name);

        m_libra_client->execute_script(account_index, (script_name += ".mv").c_str(), vector<string>{});
    }

    virtual void transfer(uint64_t account_index, uint256 address, uint64_t amount_micro_coin) override
    {
        auto script_name = m_temp_path / transfer_script;

        try_compile(script_name);

        auto args = vector<string>{uint256_to_string(address), to_string(amount_micro_coin)};

        m_libra_client->execute_script(account_index, (script_name += ".mv").c_str(), args);
    }

    virtual uint64_t get_account_balance(uint64_t account_index) override
    {
        uint64_t balance = m_libra_client->get_account_resource_uint64(account_index, m_governor_addr);
        return balance;
    }

protected:
    void init_all_script()
    {
        string governor = uint256_to_string(m_governor_addr);
        m_temp_path = fs::temp_directory_path() / uint256_to_string(m_governor_addr); ///tmp/xxxxx

        fs::remove_all(m_temp_path);

        fs::create_directory(m_temp_path);
        LOG << m_temp_path.string() << endl;

        for (auto &file : fs::directory_iterator("../scripts"))
        {
            if (file.path().extension() == ".mvir")
            {
                auto new_file = m_temp_path / file.path().filename().string();

#if __cplusplus >= 201703L
                auto option = fs::copy_options::overwrite_existing;
#else
                auto option = fs::copy_option::overwrite_if_exists;
#endif
                fs::copy_file(file.path().string(), new_file, option);
            }
        }
    }

    void try_compile(fs::path script_name, bool is_module = false)
    {
        auto mv(fs::path(script_name) += ".mv");
        auto mvir(fs::path(script_name) += ".mvir");

        if (!fs::exists(mv))
            m_libra_client->compile(m_governor_addr, mvir.c_str(), is_module);
    }

private:
    Libra::client_ptr m_libra_client;
    string m_name;
    uint256 m_governor_addr;
    string m_module;
    fs::path m_temp_path;
    const string token_module = "token";
    const string publish_script = "publish";
    const string mint_script = "mint";
    const string transfer_script = "transfer";
};

std::shared_ptr<Token> Token::create(Libra::client_ptr client,
                                     uint256 governor_addr,
                                     const std::string &name)
{
    return make_shared<TokenImp>(client, name, governor_addr);
}

} // namespace Violas

//#include <boost/python.hpp>
