#include "libra_client.hpp"
#include "rust_client_proxy.hpp"
using namespace std;
namespace Libra
{
class client_imp : virtual public client
{
protected:
    void *raw_client_proxy = nullptr;

public:
    client_imp(std::string_view host,
               ushort port,
               std::string_view validator_set_file,
               std::string_view faucet_account_file,
               bool sync_on_wallet_recovery,
               std::string_view faucet_server,
               std::string_view mnemonic_file)
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
    }

    virtual bool test_validator_connection() override
    {
        return libra_test_validator_connection((uint64_t)raw_client_proxy);
    }

    virtual std::pair<size_t, uint256> create_next_account(bool sync_with_validator) override
    {
        libra_create_next_account((uint64_t)raw_client_proxy, sync_with_validator);

        return make_pair<>(0, uint256());
    }
};

std::shared_ptr<client>
client::create(std::string_view host,
               ushort port,
               std::string_view validator_set_file,
               std::string_view faucet_account_file,
               bool sync_on_wallet_recovery,
               std::string_view faucet_server,
               std::string_view mnemonic_file)
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
    client_imp(std::string_view host,
               ushort port,
               std::string_view validator_set_file,
               std::string_view faucet_account_file,
               bool sync_on_wallet_recovery,
               std::string_view faucet_server,
               std::string_view mnemonic_file)
        : Libra::client_imp(host,
                            port,
                            validator_set_file,
                            faucet_account_file,
                            sync_on_wallet_recovery,
                            faucet_server,
                            mnemonic_file)
    {
    }

    virtual ~client_imp(){};
};

std::shared_ptr<client>
client::create(std::string_view host,
               ushort port,
               std::string_view validator_set_file,
               std::string_view faucet_account_file,
               bool sync_on_wallet_recovery,
               std::string_view faucet_server,
               std::string_view mnemonic_file)
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
