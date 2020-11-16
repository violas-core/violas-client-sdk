#include <client.hpp>
#include <sstream>
#include <iomanip>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace std;
using namespace violas;
using namespace boost::python;

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ClientWrapper_transfer_overloads, ClientWrapper::transfer_coins_int, 3, 6)

class ClientWrapper : public Client //, public wrapper<Client>
{
    violas::client_ptr m_client;

public:
    ClientWrapper(uint8_t chain_id,
                  std::string_view url,
                  std::string_view mint_key,
                  std::string_view mnemonic,
                  std::string_view waypoint)
    {
        m_client = Client::create(chain_id, url, mint_key, mnemonic, waypoint);
    }

    virtual ~ClientWrapper() {}

    virtual void
    test_connection() override
    {
        m_client->test_connection();
    }

    virtual AddressAndIndex
    create_next_account()
    {
        return m_client->create_next_account(std::nullopt);
    }

    virtual AddressAndIndex
    create_next_account(const std::optional<Address> &address = std::nullopt) override
    {
        return m_client->create_next_account(address);
    }

    virtual std::vector<Account>
    get_all_accounts() override
    {
        return m_client->get_all_accounts();
    }

    virtual void
    create_validator_account(std::string_view currency_code,
                             const AuthenticationKey &auth_key,
                             std::string_view human_name) override
    {
        m_client->create_validator_account(currency_code, auth_key, human_name);
    }

    virtual void
    mint_for_testnet(std::string_view currency_code,
                     const Address &receiver_address,
                     uint64_t amount) override
    {
        m_client->mint_for_testnet(currency_code, receiver_address, amount);
    }

    void
    mint_for_testnet_wrapper(const std::string &currency_code,
                             const Address &receiver_address,
                             uint64_t amount)
    {
        this->mint_for_testnet(currency_code, receiver_address, amount);
    }

    //
    /// transfer
    //
    virtual void
    transfer(size_t sender_account_ref_id,
             const Address &receiver_address,
             std::string_view currency_code,
             uint64_t amount,
             uint64_t gas_unit_price = 0,
             uint64_t max_gas_amount = 1000000,
             std::string_view gas_currency_code = "Coin1") override
    {
        m_client->transfer(sender_account_ref_id,
                           receiver_address,
                           currency_code,
                           amount,
                           gas_unit_price,
                           max_gas_amount,
                           gas_currency_code);
    }

    ////////////////////////////////////////////////////////////////
    //  management methods
    ////////////////////////////////////////////////////////////////

    //
    //  Allow  the custom scripts
    //  note that calling method needs violas root privilege
    virtual void
    allow_custom_script() override
    {
        m_client->allow_custom_script();
    }

    //
    //  Allow  to publish custom module
    //  note that calling method needs violas root privilege
    virtual void
    allow_publishing_module(bool enabled) override
    {
        m_client->allow_publishing_module(enabled);
    }

    //
    //  publish a module file
    //  if account_index is ASSOCIATION_ID then publish module with association root account
    virtual void
    publish_module(size_t account_index,
                   std::string_view module_file_name) override
    {
        m_client->publish_module(account_index, module_file_name);
    }

    virtual void
    execute_script_file(size_t account_index,
                        std::string_view script_file_name,
                        const std::vector<TypeTag> &type_tags = {},
                        const std::vector<TransactionAugment> &arguments = {},
                        bool is_blocking = true) override
    {
    }
    //
    //  Execute script file with specified arguments
    //
    virtual void
    execute_script(size_t account_index,
                   const std::vector<uint8_t> &script,
                   const std::vector<TypeTag> &type_tags = {},
                   const std::vector<TransactionAugment> &arguments = {},
                   bool is_blocking = true) override
    {
    }

    ///////////////////////////////////////////////////////
    // multi-currency method
    ///////////////////////////////////////////////////////

    // Call this method with root association privilege
    virtual void
    publish_curency(std::string_view currency_code) override
    {
        m_client->publish_curency(currency_code);
    }

    void publish_curency_wrapper(const std::string &currency_code)
    {
        m_client->publish_curency(currency_code);
    }

    // Register currency with association root account
    virtual void
    register_currency(std::string_view currency_code,
                      uint64_t exchange_rate_denom,
                      uint64_t exchange_rate_num,
                      bool is_synthetic,
                      uint64_t scaling_factor,
                      uint64_t fractional_part) override
    {
        m_client->register_currency(currency_code,
                                    exchange_rate_denom,
                                    exchange_rate_num,
                                    is_synthetic,
                                    scaling_factor,
                                    fractional_part);
    }

    void
    register_currency_wrapper(const std::string &currency_code,
                              uint64_t exchange_rate_denom,
                              uint64_t exchange_rate_num,
                              bool is_synthetic,
                              uint64_t scaling_factor,
                              uint64_t fractional_part)
    {
        m_client->register_currency(currency_code,
                                    exchange_rate_denom,
                                    exchange_rate_num,
                                    is_synthetic,
                                    scaling_factor,
                                    fractional_part);
    }

    /// add currency for the designated dealer account
    virtual void
    add_currency_for_designated_dealer(
        std::string_view currency_code,
        const Address &dd_address) override
    {
        m_client->add_currency_for_designated_dealer(currency_code, dd_address);
    }

    void
    add_currency_for_designated_dealer_wrapper(
        const std::string &currency_code,
        const Address &dd_address)
    {
        m_client->add_currency_for_designated_dealer(currency_code, dd_address);
    }

    /// Add a currency to current account
    virtual void
    add_currency(size_t account_index,
                 std::string_view currency_code) override
    {
        m_client->add_currency(account_index, currency_code);
    }

    void
    add_currency_wrapper(size_t account_index,
                         const std::string &currency_code)
    {
        m_client->add_currency(account_index, currency_code);
    }

    /// mint currency for dd account
    virtual void
    mint_currency_for_designated_dealer(
        std::string_view currency_code,
        uint64_t sliding_nonce,
        const Address &dd_address,
        uint64_t amount,
        uint64_t tier_index) override
    {
        m_client->mint_currency_for_designated_dealer(
            currency_code,
            sliding_nonce,
            dd_address,
            amount,
            tier_index);
    }

    void mint_currency_for_designated_dealer_wrapper(
        const std::string &currency_code,
        uint64_t sliding_nonce,
        const Address &dd_address,
        uint64_t amount,
        uint64_t tier_index)
    {
        m_client->mint_currency_for_designated_dealer(
            currency_code,
            sliding_nonce,
            dd_address,
            amount,
            tier_index);
    }

    virtual void
    create_designated_dealer_account(
        std::string_view currency_code,
        uint64_t nonce,
        const Address &new_account_address,
        const AuthenticationKey &auth_key,
        std::string_view human_name,
        std::string_view base_url,
        PublicKey compliance_public_key,
        bool add_all_currencies) override
    {
    }

    virtual void
    update_account_authentication_key(const Address &address,
                                      const AuthenticationKey &auth_key) override
    {
        m_client->update_account_authentication_key(address, auth_key);
    }

    virtual void
    update_dual_attestation_limit(uint64_t sliding_nonce, uint64_t new_micro_lbr_limit) override
    {
        m_client->update_dual_attestation_limit(sliding_nonce, new_micro_lbr_limit);
    }

    virtual void
    rotate_authentication_key_with_nonce(size_t account_index,
                                         uint64_t sliding_nonce,
                                         const AuthenticationKey &new_auth_key,
                                         bool is_blocking) override
    {
    }
    
    //
    // Create parent VASP account
    //
    virtual void
    create_parent_vasp_account(std::string_view currency_code,
                               uint64_t nonce,
                               const Address &new_account_address,
                               const AuthenticationKey &auth_key,
                               std::string_view human_name,
                               std::string_view base_url,
                               PublicKey compliance_public_key,
                               bool add_all_currencies) override
    {
        m_client->create_parent_vasp_account(currency_code,
                                             nonce,
                                             new_account_address,
                                             auth_key,
                                             human_name,
                                             base_url,
                                             compliance_public_key,
                                             add_all_currencies);
    }

    void
    create_parent_vasp_account_wrapper(std::string currency_code,
                                       uint64_t nonce,
                                       const Address &new_account_address,
                                       const AuthenticationKey &auth_key,
                                       std::string human_name,
                                       std::string base_url,
                                       PublicKey compliance_public_key,
                                       bool add_all_currencies)
    {
        this->create_parent_vasp_account(currency_code,
                                         nonce,
                                         new_account_address,
                                         auth_key,
                                         human_name,
                                         base_url,
                                         compliance_public_key,
                                         add_all_currencies);
    }

    /**
         * @brief Query accout status infomation
         * 
         * @param address - the address of account
         * @return std::string 
         */
    virtual std::string
    query_account_info(const Address &address) override
    {
        return m_client->query_account_info(address);
    }

    /**
         * @brief Query transaction inforamtion by address and seqence number
         * 
         * @param address the address of account
         * @param seq_number    the sequence number of account
         * @param is_fetching_event whether fectching event or not
         * @return std::string with json format
         */
    virtual std::string
    query_transaction_info(const Address &address,
                           uint64_t sequence_number,
                           bool is_fetching_events) override
    {
        return m_client->query_transaction_info(address, sequence_number, is_fetching_events);
    }
    /**
         * @brief Query transaction inforamtion by range
         * 
         * @param start_version     start version 
         * @param limit             limit of range, amount of queried transaction
         * @param is_fetching_event whether fectching event or not
         * @return std::string  with json format
         */
    virtual std::string
    query_transaction_info(uint64_t start_version,
                           uint64_t limit,
                           bool is_fetching_events) override
    {
        return m_client->query_transaction_info(start_version, limit, is_fetching_events);
    }

    virtual std::string
    query_events(const Address &address,
                 event_type type,
                 uint64_t start_version,
                 uint64_t limit) override
    {
        return m_client->query_events(address, type, start_version, limit);
    }

    /// get the balance of currency for the account address
    virtual uint64_t
    get_currency_balance(const Address &address,
                         std::string_view currency_code) override
    {
        return m_client->get_currency_balance(address, currency_code);
    }
    virtual std::string
    get_all_currency_info() override
    {
        return m_client->get_all_currency_info();
    }

    //  Exchnage interface
    //
    virtual std::string
    get_exchange_currencies(const Address &address) override
    {
        return m_client->get_exchange_currencies(address);
    }

    //
    //  get exchange reservers
    //
    virtual std::string
    get_exchange_reserves(const Address &address) override
    {
        return m_client->get_exchange_reserves(address);
    }

    //
    //  get liquidity balance
    //
    virtual std::string
    get_liquidity_balance(const Address &address) override
    {
        return m_client->get_liquidity_balance(address);
    }
};

template <size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<uint8_t, N> &bytes)
{
    for (auto v : bytes)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

template <typename T>
string to_str(const T &t)
{
    ostringstream iss;

    iss << t;

    return iss.str();
}

BOOST_PYTHON_MODULE(libpy_violas_sdk)
{
    using namespace violas;

    class_<Address>("Address")
        .add_property(
            "str", +[](const Address &addr) { return to_str(addr); });

    boost::python::scope().attr("TESTNET_DD_ADDRESS") = TESTNET_DD_ADDRESS;

    class_<AuthenticationKey>("AuthenticationKey")
        .add_property(
            "str",
            +[](const AuthenticationKey &auth_key) -> string { return to_str(auth_key); });

    class_<AddressAndIndex>("AddressAndIndex")
        .add_property("index", &AddressAndIndex::index)
        .add_property("address", &AddressAndIndex::address);

    enum_<AccountStatus>("AccountStatus")
        .value("Local", Local)
        .value("Persisted", Persisted)
        .value("Unknow", Unknow);

    class_<Account>("Account")
        .add_property("index", &Account::index)
        .add_property("address", &Account::address)
        .add_property("auth_key", &Account::auth_key)
        .add_property("pub_key", &Account::pub_key)
        .add_property("sequence_number", &Account::sequence_number)
        .add_property("status", &Account::status);

    using Accounts = std::vector<Account>;
    class_<Accounts>("Accounts")
        .def(vector_indexing_suite<Accounts>());

    // void (ClientWrapper::*compile1)(uint64_t, const string &, bool) = &ClientWrapper::compile;
    AddressAndIndex (ClientWrapper::*create_next_account_ptr)() = &ClientWrapper::create_next_account;

    class_<ClientWrapper, std::shared_ptr<ClientWrapper>>("Client", init<uint8_t, string, string, string, string>())
        .def("test_connection", &ClientWrapper::test_connection)
        .def("create_next_account", create_next_account_ptr)
        .def("get_all_accounts", &ClientWrapper::get_all_accounts)
        .def("mint_for_testnet", &ClientWrapper::mint_for_testnet_wrapper)
        .def("transfer", &ClientWrapper::transfer)
        .def("publish_module", &ClientWrapper::publish_module)
        .def("execute_script", &ClientWrapper::execute_script)
        .def("publish_currency", &ClientWrapper::publish_curency_wrapper)
        .def("register_currency", &ClientWrapper::register_currency_wrapper)
        .def("add_currency", &ClientWrapper::add_currency_wrapper)
        .def("add_currency_for_designated_dealer", &ClientWrapper::add_currency_for_designated_dealer_wrapper)
        .def("mint_currency_for_designated_dealer", &ClientWrapper::mint_currency_for_designated_dealer_wrapper)
        .def("create_designated_dealer_account", &ClientWrapper::create_designated_dealer_account)
        .def("create_parent_vasp_account", &ClientWrapper::create_parent_vasp_account_wrapper)
        //     .def("transfer", &ClientWrapper::transfer_coins_int, ClientWrapper_transfer_overloads())
        ;

    // uint64_t (TokenImp::*get_account_balance1)(uint64_t) = &TokenImp::get_account_balance;
    // uint64_t (TokenImp::*get_account_balance2)(uint256) = &TokenImp::get_account_balance;

    // class_<TokenImp, std::shared_ptr<TokenImp>>("Token", init<client_ptr, uint256, string, string>())
    //     .def("name", &TokenImp::name)
    //     .def("address", &TokenImp::address)
    //     .def("deploy", &TokenImp::deploy)
    //     .def("mint", &TokenImp::mint)
    //     .def("publish", &TokenImp::publish)
    //     .def("transfer", &TokenImp::transfer)
    //     .def("get_account_balance", get_account_balance1)
    //     .def("get_account_balance", get_account_balance2);

    implicitly_convertible<std::shared_ptr<ClientWrapper>, std::shared_ptr<Client>>();
}