#ifndef LIBRA_CLIENT
#define LIBRA_CLIENT

#include <memory>
#include <array>
#include <vector>
#include <iostream>
//
//  log
//
std::ostream &log(std::ostream &ost, const char *flag, const char *file, int line, const char *func);

#define LOG log(clog, "[ LOG   ] ", __FILE__, __LINE__, __func__)
#define ERROR log(cerr, "[ ERROR ] ", __FILE__, __LINE__, __func__)
///
/// uint256
///
using uint256 = std::array<uint8_t, 32>;

std::ostream &operator<<(std::ostream &os, const uint256 &value);
std::ostream &operator>>(std::ostream &os, const uint256 &value);

std::string uint256_to_string(const uint256 &address);
uint256 uint256_from_string(const std::string &str_addr);

const uint64_t micro_libra_coin = 1000000;

namespace Libra
{
///
/// Libra client
///
class client
{
public:
    static std::shared_ptr<client>
    create(const std::string &host,
           ushort port,
           const std::string &validator_set_file,
           const std::string &faucet_account_file,
           bool sync_on_wallet_recovery,
           const std::string &faucet_server,
           const std::string &mnemonic_file);

    virtual ~client(){};

    virtual void test_validator_connection() = 0;

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

    /// Transfer num_coins from sender account to receiver.
    //  If is_blocking = true, it will keep querying validator till the sequence number is bumped up in validator.
    //  return : account's index and sequence number
    virtual std::pair<uint64_t, uint64_t>
    transfer_coins_int(uint64_t sender_account_ref_id, // the reference id of account
                       uint256 receiver_address,       // the address of receiver
                       uint64_t micro_coins,           // a millionth of a coin
                       uint64_t gas_unit_price,        // set gas unit price or 0
                       uint max_gas_amount,            // set the max gas account or 0
                       bool is_blocking) = 0;          // true for sync, fasle for async

    virtual void compile(uint64_t account_index, const std::string &source_file, bool is_module = false) = 0;

    virtual void publish_module(uint64_t account_index, const std::string &module_file) = 0;

    virtual void execute_script(uint64_t account_index, const std::string &script_file, const std::vector<std::string> &script_args) = 0;

    virtual void get_committed_txn_by_acc_seq(uint64_t account_index, uint64_t sequence_num) = 0;
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
    create(const std::string &host,
           ushort port,
           const std::string &validator_set_file,
           const std::string &faucet_account_file,
           bool sync_on_wallet_recovery,
           const std::string &faucet_server,
           const std::string &mnemonic_file);

    virtual ~client(){};

    virtual uint64_t get_violas_balance(uint64_t account_index) = 0;
};

using client_ptr = std::shared_ptr<client>;

} // namespace Violas

#endif