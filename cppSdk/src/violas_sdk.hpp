#ifndef LIBRA_CLIENT
#define LIBRA_CLIENT

#ifndef LIB_NAME
#define LIB_NAME Violas
#endif

#include <memory>
#include <array>
#include <vector>
#include <iostream>
#include <functional>

//
//  log
//
std::ostream &
log(std::ostream &ost, const char *flag, const char *file, int line, const char *func);

#define COUT log(cout, "[ INFO  ] ", __FILE__, __LINE__, __func__)
#define CLOG log(clog, "[ LOG   ] ", __FILE__, __LINE__, __func__)
#define CERR log(cerr, "[ ERROR ] ", __FILE__, __LINE__, __func__)

template <typename... Args>
std::string format(const std::string &format, Args... args)
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);

    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

namespace LIB_NAME
{

std::string tx_vec_data(const std::string &data);

const uint64_t MICRO_LIBRO_COIN = 1000000;

bool is_valid_balance(uint64_t value);
//
//  Address
//
class Address
{
public:
    Address()
    {
        m_data = {0};
    }
    Address(const uint8_t *data, uint64_t len);

    std::string to_string() const;
    static Address from_string(const std::string &hex_addr);

    friend std::ostream &operator<<(std::ostream &os, const Address &address);
    friend std::istream &operator>>(std::istream &is, Address &address);
    bool operator==(const Address &right) const;

    const auto &data() const { return m_data; };

    static const uint64_t length = 16;

private:
    std::array<uint8_t, length> m_data;
};

// replace mv with addr
void replace_mv_with_addr(const std::string &mv_file_name,
                          const std::string &new_file_name,
                          const Address &address);

//
//  interface Client
//
class Client
{
public:
    static std::shared_ptr<Client>
    create(const std::string &host,
           uint16_t port,
           const std::string &validator_set_file,
           const std::string &faucet_account_file,
           bool sync_on_wallet_recovery,
           const std::string &faucet_server,
           const std::string &mnemonic_file);

    virtual ~Client(){};

    virtual void test_validator_connection() = 0;

    /// Create a new account
    /// return the index and address of account
    virtual std::pair<size_t, Address>
    create_next_account(bool sync_with_validator) = 0;

    struct Account
    {
        uint64_t index;
        Address address;
        uint64_t sequence_number;
        int64_t status;

        bool operator==(const Account &r) const
        {
            return index == r.index && address == r.address;
        }
    };

    virtual std::vector<Account> get_all_accounts() = 0;

    virtual double get_balance(uint64_t index) = 0;

    virtual double get_balance(const Address &address) = 0;

    virtual uint64_t get_sequence_number(uint64_t index) = 0;
    virtual uint64_t get_sequence_number(const Address &address) = 0;

    virtual void mint_coins(uint64_t index, uint64_t num_coins, bool is_blocking = true) = 0;

    /// Transfer num_coins from sender account to receiver.
    //  If is_blocking = true, it will keep querying validator till the sequence number is bumped up in validator.
    //  return : account's index and sequence number
    virtual std::pair<uint64_t, uint64_t>
    transfer_coins_int(uint64_t sender_account_ref_id, // the reference id of account
                       Address receiver_address,       // the address of receiver
                       uint64_t micro_coins,           // a millionth of a coin
                       uint64_t gas_unit_price = 0,    // set gas unit price or 0
                       uint64_t max_gas_amount = 0,    // set the max gas account or 0
                       bool is_blocking = true) = 0;   // true for sync, fasle for async

    virtual void
    compile(uint64_t account_index,
            const std::string &source_file,
            bool is_module = false,
            const std::string &temp_dir = "") = 0;

    virtual void
    compile(Address account_address,
            const std::string &source_file,
            bool is_module = false,
            const std::string &temp_dir = "") = 0;

    virtual void publish_module(uint64_t account_index, const std::string &module_file) = 0;

    virtual void execute_script(uint64_t account_index, const std::string &script_file,
                                const std::vector<std::string> &script_args) = 0;

    virtual std::pair<std::string, std::string>
    get_committed_txn_by_acc_seq(uint64_t account_index, uint64_t sequence_num) = 0;

    virtual std::pair<std::string, std::string>
    get_committed_txn_by_acc_seq(Address address, uint64_t sequence_num) = 0;

    virtual std::vector<std::pair<std::string, std::string>>
    get_txn_by_range(uint64_t start_version, uint64_t limit, bool fetch_events) = 0;

    //
    //  get unsigned int 64 resource of a account
    //  account_index : the index of account
    //  res_path : the path of resouce, usually the format is address.module.struct
    //
    virtual uint64_t
    get_account_resource_uint64(uint64_t account_index, const Address &res_path_addr, uint64_t token_index) = 0;

    virtual uint64_t
    get_account_resource_uint64(const Address &account_addr, const Address &res_path_addr, uint64_t token_index) = 0;
};

using client_ptr = std::shared_ptr<Client>;
//
//  Interface Token
//
class Token
{
public:
    static std::shared_ptr<Token>
    create(client_ptr client,
           Address governor_addr,
           const std::string &name,
           const std::string &script_files_path = "../scripts");

    static std::shared_ptr<Token>
    create(client_ptr client,
           Address governor_addr,
           const std::string &name,
           std::function<void(const std::string &)> init_all_script_fun,
           const std::string &temp_path);

    virtual ~Token() {}

    virtual std::string name() = 0;

    virtual Address address() = 0;

    virtual void deploy(uint64_t account_index) = 0;

    virtual void publish(uint64_t account_index, const std::string &user_data = "publish") = 0;

    virtual void create_token(uint64_t account_index, Address owner, const std::string &token_data = "token") = 0;

    virtual void mint(uint64_t account_index,
                      uint64_t token_index,
                      Address receiver,
                      uint64_t amount_micro_coin,
                      const std::string &data = "mint") = 0;

    virtual void transfer(uint64_t account_index,
                          uint64_t token_index,
                          Address receiver,
                          uint64_t amount_micro_coin,
                          const std::string &data = "transfer") = 0;

    virtual uint64_t get_account_balance(uint64_t account_index, uint64_t token_index) = 0;

    virtual uint64_t get_account_balance(Address account_address, uint64_t token_index) = 0;
};

using token_ptr = std::shared_ptr<Token>;

} // namespace LIB_NAME

#endif