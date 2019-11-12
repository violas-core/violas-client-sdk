#include <iomanip>
#include <ctime>
//#include <chrono>
#include <sstream>
#include "libra_client.hpp"
#include "rust_client_proxy.hpp"

using namespace std;

inline ostream &log(const char *file, int line, const char *func)
{
    time_t now = time(nullptr);

    clog << put_time(std::localtime(&now), "%F %T") << " (" << file << ":" << line << ":" << func << ") : ";

    return clog;
}

#define LOG log(__FILE__, __LINE__, __func__)

ostream &operator<<(ostream &os, const uint256 &value)
{
    for (auto v : value)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os;
}

std::ostream &operator>>(std::ostream &os, const uint256 &value)
{
    return os;
}

namespace Libra
{
class client_imp : virtual public client
{
protected:
    void *raw_client_proxy = nullptr;

public:
    client_imp(const std::string &host,
               ushort port,
               const std::string &validator_set_file,
               const std::string &faucet_account_file,
               bool sync_on_wallet_recovery,
               const std::string &faucet_server,
               const std::string &mnemonic_file)
    {
        raw_client_proxy = (void *)create_libra_client_proxy(host.data(),
                                                             port,
                                                             validator_set_file.data(),
                                                             faucet_account_file.data(),
                                                             sync_on_wallet_recovery,
                                                             faucet_server.data(),
                                                             mnemonic_file.data());
        if (raw_client_proxy == nullptr)
            throw runtime_error("failed to create native rust client proxy");
    }

    virtual ~client_imp()
    {
        destory_libra_client_proxy((uint64_t)raw_client_proxy);

        LOG << "entered" << endl;
    }

    virtual void test_validator_connection() override
    {
        bool ret = libra_test_validator_connection((uint64_t)raw_client_proxy);
        if (!ret)
            throw runtime_error("failed to test validator connection");
    }

    virtual std::pair<size_t, uint256> create_next_account(bool sync_with_validator) override
    {
        Address account = libra_create_next_account((uint64_t)raw_client_proxy, sync_with_validator);

        uint256 address;
        copy(begin(account.address), end(account.address), begin(address));

        return make_pair<>(account.index, address);
    }

    virtual std::vector<Account> get_all_accounts() override
    {
        Accounts all_accounts = libra_get_all_accounts((uint64_t)raw_client_proxy);

        vector<Account> accounts;

        for (int i = 0; i < all_accounts.len; i++)
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

    virtual void mint_coins(uint64_t index, uint64_t num_coins, bool is_blocking) override
    {
        libra_mint_coins((uint64_t)raw_client_proxy, index, num_coins, is_blocking);
    }

    virtual std::pair<uint64_t, uint64_t>
    transfer_coins_int(uint64_t sender_account_ref_id,
                       uint256 receiver_address,
                       uint64_t num_coins,
                       uint64_t gas_unit_price,
                       uint max_gas_amount,
                       bool is_blocking) override
    {
        _index_sequence index_seq;
        bool ret = libra_transfer_coins_int((uint64_t)raw_client_proxy,
                                            sender_account_ref_id,
                                            receiver_address.data(),
                                            num_coins,
                                            gas_unit_price,
                                            max_gas_amount,
                                            is_blocking,
                                            &index_seq);
        if (!ret)
            throw runtime_error("failed to transfer coins");

        return make_pair(index_seq.index, index_seq.sequence);
    }

    virtual void compile(uint64_t account_index, const string &source_file_with_path, bool is_module) override
    {
        // auto accounts = get_all_accounts();

        // ostringstream oss;

        // oss << accounts[account_index].address;

        bool ret = libra_compile((uint64_t)raw_client_proxy, account_index, source_file_with_path.c_str(), is_module);
        if (!ret)
            throw runtime_error("failed to compile move script file");
    }

    virtual void publish_module(uint64_t account_index, const std::string &module_file) override
    {
        bool ret = libra_publish_module((uint64_t)raw_client_proxy, account_index, module_file.c_str());
        if (!ret)
            throw runtime_error("failed to publish module file");
    }

    virtual void execute_script(uint64_t account_index, const std::string &script_file, const std::vector<std::string> &script_args) override
    {
        ScriptArgs args;

        vector<const char *> args_array;
        for (auto &arg : script_args)
        {
            args_array.push_back(arg.c_str());
        }

        args.len = script_args.size();
        args.data = args_array.data();

        bool ret = libra_execute_script((uint64_t)raw_client_proxy, account_index, script_file.c_str(), &args);
        if (!ret)
            throw runtime_error("failed to publish module file");
    }
};

std::shared_ptr<client>
client::create(const std::string &host,
               ushort port,
               const std::string &validator_set_file,
               const std::string &faucet_account_file,
               bool sync_on_wallet_recovery,
               const std::string &faucet_server,
               const std::string &mnemonic_file)
{
    return make_shared<client_imp>(host,
                                   port,
                                   validator_set_file,
                                   faucet_account_file,
                                   sync_on_wallet_recovery,
                                   faucet_server,
                                   mnemonic_file);
}
} // namespace Libra

namespace Violas
{
class client_imp : virtual public client, virtual public Libra::client_imp
{
private:
    /* data */
public:
    client_imp(const std::string &host,
               ushort port,
               const std::string &validator_set_file,
               const std::string &faucet_account_file,
               bool sync_on_wallet_recovery,
               const std::string &faucet_server,
               const std::string &mnemonic_file)
        : Libra::client_imp(host,
                            port,
                            validator_set_file,
                            faucet_account_file,
                            sync_on_wallet_recovery,
                            faucet_server,
                            mnemonic_file)
    {
    }

    virtual ~client_imp()
    {
        LOG << " entered" << endl;
    };
};

std::shared_ptr<client>
client::create(const std::string &host,
               ushort port,
               const std::string &validator_set_file,
               const std::string &faucet_account_file,
               bool sync_on_wallet_recovery,
               const std::string &faucet_server,
               const std::string &mnemonic_file)
{
    return make_shared<client_imp>(host,
                                   port,
                                   validator_set_file,
                                   faucet_account_file,
                                   sync_on_wallet_recovery,
                                   faucet_server,
                                   mnemonic_file);
}

} // namespace Violas
