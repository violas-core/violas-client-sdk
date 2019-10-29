#ifndef LIBRA_CLIENT
#define LIBRA_CLIENT

#include <string_view>
#include <memory>
#include <array>
#include <vector>
#include <iostream>

using uint256 = std::array<uint8_t, 32>;
std::ostream &operator<<(std::ostream &os, const uint256 &value);

namespace Libra
{
///
/// Libra client
///
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

    struct Account
    {
        uint256 address;
        uint64_t index;
        uint64_t sequence_number;
        int64_t status;
    };

    virtual std::vector<Account> get_all_accounts() = 0;

    virtual double get_balance(uint64_t index) = 0;

    virtual uint64_t get_sequence_number(uint64_t index) = 0;

    virtual void mint_coins(uint64_t index, uint64_t num_coins, bool is_blocking) = 0;
};

using client_ptr = std::shared_ptr<client>;

} // namespace Libra

namespace Violas
{
///
/// Vioals client
///
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