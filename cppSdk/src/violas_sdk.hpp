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
#include <string_view>
//#include <sstream>
#include <iomanip>
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

std::string tx_vec_data(const std::string &data);

//
//  Address
//
template <size_t N>
class Bytes
{
public:
    Bytes()
    {
        m_data = {0};
    }

    Bytes(const uint8_t *data, uint64_t len)
    {
        if (len > N)
            len = N;

        std::copy(data, data + len, m_data.data());
    }

    Bytes(const Bytes &bytes)
    {
        using namespace std;
        copy(begin(bytes.data()), end(bytes.data()), begin(m_data));
    }

    Bytes &operator=(const Bytes &bytes)
    {
        using namespace std;
        copy(begin(bytes.data()), end(bytes.data()), begin(m_data));

        return *this;
    }

    static Bytes from_string(const std::string &hex)
    {
        std::array<uint8_t, N> data;
        auto iter = begin(data);

        for (size_t i = 0; i < hex.size() && i < N * 2; i += 2)
        {
            std::istringstream iss(hex.substr(i, 2));
            uint32_t byte;

            iss >> std::hex >> std::setw(2) >> byte; // std::setfill('0') >> std::setw(2)
            *iter++ = byte;
        }

        return Bytes(data.data(), data.size());
    }

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << *this;

        return oss.str();
    }

    //friend std::ostream &operator<<(std::ostream &os, const Bytes &address);

    //friend std::istream &operator>>(std::istream &is, Bytes &address);

    bool operator==(const Bytes &right) const
    {
        return m_data == right.m_data;
    }

    static const std::size_t length = N;
    //using LENGTH = N;

    const auto &data() const { return m_data; };

private:
    std::array<uint8_t, N> m_data;
};

template <size_t N>
std::ostream &operator<<(std::ostream &os, const Bytes<N> &bytes)
{
    for (auto v : bytes.data())
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

template <size_t N>
std::istream &operator>>(std::istream &is, Bytes<N> &address)
{
    return is;
}

namespace LIB_NAME
{
    using Address = Bytes<16>;
    using AuthenticationKey = Bytes<32>;

    struct TypeTag
    {
        Address address;
        std::string module;
        std::string res_name;

        TypeTag(Address addr,
                std::string_view mod,
                std::string_view name) : address(addr),
                                         module(mod),
                                         res_name(name)
        {
        }
    };

    bool is_valid_balance(uint64_t value);

    // replace mv with addr
    void replace_mv_with_addr(const std::string &mv_file_name,
                              const std::string &new_file_name,
                              const Address &address);

    const uint64_t MICRO_COIN = 1000000;
    const uint64_t ASSOCIATION_ID = std::numeric_limits<uint64_t>::max();
    const Address ASSOCIATION_ADDRESS = Address::from_string("0000000000000000000000000A550C18");
    const Address CORE_CODE_ADDRESS = Address();
    //
    //  interface Client
    //
    class Client
    {
    public:
        static std::shared_ptr<Client>
        create(const std::string &url,
               const std::string &mint_key_file_name,
               bool sync_on_wallet_recovery,
               const std::string &faucet_server,
               const std::string &mnemonic_file_name,
               const std::string &waypoint);

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
            AuthenticationKey auth_key;
            uint64_t sequence_number;
            int64_t status;

            bool operator==(const Account &r) const
            {
                return index == r.index && address == r.address;
            }
        };

        virtual std::vector<Account> get_all_accounts() = 0;

        virtual uint64_t get_balance(uint64_t index) = 0;

        virtual uint64_t get_balance(const Address &address) = 0;

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

        /// execute script
        virtual void execute_script(uint64_t account_index,
                                    std::string_view script_file,
                                    const std::vector<std::string> &script_args = std::vector<std::string>()) = 0;

        /// execute script with TypeTag
        virtual void execute_script(const TypeTag &tag,
                                    uint64_t account_index,
                                    std::string_view script_file,
                                    const std::vector<std::string> &script_args = std::vector<std::string>()) = 0;

        virtual std::string
        get_committed_txn_by_acc_seq(uint64_t account_index, uint64_t sequence_num, bool fetch_event = true) = 0;

        virtual std::string
        get_committed_txn_by_acc_seq(Address address, uint64_t sequence_num, bool fetch_event = true) = 0;

        virtual std::vector<std::pair<std::string, std::string>>
        get_txn_by_range(uint64_t start_version, uint64_t limit, bool fetch_events) = 0;

        enum EventType
        {
            sent = true,
            received = false
        };
        //
        /// Get events by account index and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(uint64_t account_index, EventType type, uint64_t start_seq_number, uint64_t limit) = 0;
        //
        // Get events by account and event type with start sequence number and limit.
        //
        virtual std::pair<std::vector<std::string>, std::string>
        get_events(Address address, EventType type, uint64_t start_seq_number, uint64_t limit) = 0;

        //
        //  get unsigned int 64 resource of a account
        //  account_index : the index of account
        //  res_path : the path of resouce, usually the format is address.module.struct
        //
        virtual uint64_t
        get_account_resource_uint64(uint64_t account_index, const Address &res_path_addr, uint64_t token_index) = 0;

        virtual uint64_t
        get_account_resource_uint64(const Address &account_addr, const Address &res_path_addr, uint64_t token_index) = 0;

        virtual void
        enable_custom_script(bool is_enabled) = 0;

        /// add a currency to current account
        virtual void
        add_currency(const TypeTag &type_tag, uint64_t account_index, bool is_blocking = true) = 0;

        /// pulish a currency module wiht 0x0 address
        virtual void 
        publish_currency(std::string_view currency_code) = 0;

        /// register a new currency to blockchain
        virtual void
        register_currency(const TypeTag &type_tag,
                          uint64_t exchange_rate_denom,
                          uint64_t exchange_rate_num,
                          bool is_synthetic,
                          uint64_t scaling_factor,
                          uint64_t fractional_part,
                          std::string_view currency_code) = 0;

        /// mint currency for a receiver
        virtual void
        mint_currency(const TypeTag &tag,
                      const AuthenticationKey &receiver_auth_key,
                      uint64_t amount,
                      bool is_blocking = true) = 0;

        /// transfer currency to a receiver
        virtual void
        transfer_currency(const TypeTag &tag,
                          uint64_t sender_account_index,
                          const AuthenticationKey &receiver_auth_key,
                          uint64_t amount,
                          bool is_blocking = true) = 0;

        /// get balance of currency
        //
        virtual uint64_t
        get_currency_balance(const TypeTag &tag, const Address &address, bool throw_excption = false) = 0;

        // get currency info
        virtual std::string get_currency_info() = 0;

        // get account state
        virtual std::pair<std::string, uint64_t>
        get_account_state(const Address &res_path_addr) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
    //
    //  Interface Token
    //
    class TokenManager
    {
    public:
        static std::shared_ptr<TokenManager>
        create(client_ptr client,
               Address supervisor_addr,
               const std::string &name,
               const std::string &script_files_path = "../scripts");

        static std::shared_ptr<TokenManager>
        create(client_ptr client,
               Address supervisor_addr,
               const std::string &name,
               std::function<void(const std::string &)> init_all_script_fun,
               const std::string &temp_path);

        virtual ~TokenManager() {}

        virtual std::string name() = 0;

        virtual Address address() = 0;

        virtual void deploy(uint64_t account_index) = 0;

        virtual void publish(uint64_t account_index, const std::string &user_data = "publish") = 0;

        virtual void create_token(uint64_t supervisor, Address owner, const std::string &token_data = "token") = 0;

        virtual void mint(uint64_t token_index,
                          uint64_t account_index,
                          Address receiver,
                          uint64_t amount_micro_coin,
                          const std::string &data = "mint") = 0;

        virtual void transfer(uint64_t token_index,
                              uint64_t account_index,
                              Address receiver,
                              uint64_t amount_micro_coin,
                              const std::string &data = "transfer") = 0;

        virtual uint64_t token_count() = 0;

        virtual uint64_t get_account_balance(uint64_t token_index, uint64_t account_index) = 0;

        virtual uint64_t get_account_balance(uint64_t token_index, Address account_address) = 0;
    };

    using token_manager_ptr = std::shared_ptr<TokenManager>;

} // namespace LIB_NAME

#endif