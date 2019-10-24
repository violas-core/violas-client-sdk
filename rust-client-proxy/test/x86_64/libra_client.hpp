#ifndef LIBRA_CLIENT
#define LIBRA_CLIENT

#include <string_view>
#include <memory>
#include <array>

using uint256 = std::array<uint8_t, 32>;

///
/// libra_client interface
///
namespace Libra
{
class client
{
public:
    static std::shared_ptr<client>
    create(std::string_view host,
           ushort port,
           std::string_view validator_set_file,
           std::string_view faucet_account_file,
           bool sync_on_wallet_recovery,
           std::string_view faucet_server,
           std::string_view mnemonic_file);

    virtual ~client(){};

    virtual bool test_validator_connection() = 0;

    /// Create a new account
    /// return the index and address of account
    virtual std::pair<size_t, uint256> create_next_account(bool sync_with_validator) = 0;
};

using client_ptr = std::shared_ptr<client>;

} // namespace Libra

///
/// Vioals client
///
namespace Violas
{
class client : virtual public Libra::client
{
public:
    static std::shared_ptr<Violas::client>
    create(std::string_view host,
           ushort port,
           std::string_view validator_set_file,
           std::string_view faucet_account_file,
           bool sync_on_wallet_recovery,
           std::string_view faucet_server,
           std::string_view mnemonic_file);

    virtual ~client(){};
};

using client_ptr = std::shared_ptr<client>;

} // namespace Violas

#endif