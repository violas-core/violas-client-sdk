#include "libra_client.hpp"
#include "rust_client_proxy.hpp"
using namespace std;

class libra_client_imp : public libra_client
{
private:
    /* data */
    void *raw_client_proxy = nullptr;

public:
    libra_client_imp(std::string_view host,
                     ushort port,
                     std::string_view validator_set_file,
                     std::string_view faucet_account_file,
                     bool sync_on_wallet_recovery,
                     std::string_view faucet_server,
                     std::string_view mnemonic_file);
    virtual ~libra_client_imp();
};

libra_client_imp::libra_client_imp(std::string_view host,
                                   ushort port,
                                   std::string_view validator_set_file,
                                   std::string_view faucet_account_file,
                                   bool sync_on_wallet_recovery,
                                   std::string_view faucet_server,
                                   std::string_view mnemonic_file)
{
    raw_client_proxy = (void *)create_native_client_proxy(host.data(),
                                                          port,
                                                          validator_set_file.data(),
                                                          faucet_account_file.data(),
                                                          sync_on_wallet_recovery,
                                                          faucet_server.data(),
                                                          mnemonic_file.data());
    if (raw_client_proxy == nullptr)
        throw runtime_error("failed to create native rust client proxy");
}

libra_client_imp::~libra_client_imp()
{
}

std::shared_ptr<libra_client>
libra_client::create_libra_client(std::string_view host,
                                  ushort port,
                                  std::string_view validator_set_file,
                                  std::string_view faucet_account_file,
                                  bool sync_on_wallet_recovery,
                                  std::string_view faucet_server,
                                  std::string_view mnemonic_file)
{
    return make_shared<libra_client_imp>(host,
                                         port,
                                         validator_set_file,
                                         faucet_account_file,
                                         sync_on_wallet_recovery,
                                         faucet_server,
                                         mnemonic_file);
}