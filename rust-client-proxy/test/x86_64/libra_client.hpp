#ifndef LIBRA_CLIENT
#define LIBRA_CLIENT

#include <string_view>
#include <memory>

struct libra_client
{
    static std::shared_ptr<libra_client>
    create_libra_client(std::string_view host,
                        ushort port,
                        std::string_view validator_set_file,
                        std::string_view faucet_account_file,
                        bool sync_on_wallet_recovery,
                        std::string_view faucet_server,
                        std::string_view mnemonic_file);

    virtual ~libra_client(){};
};

using libra_client_ptr = std::shared_ptr<libra_client>;

#endif