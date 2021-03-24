/**
 * @file client.hpp
 * @author Hunter Sun (HunterSun2018@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#if __cplusplus < 201703
#error c++ complier must be greater than c++17
#endif

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <limits>
#include <variant>
#include <optional>

namespace violas
{
    const size_t ADDRESS_LENGTH = 16;
    using Address = std::array<uint8_t, ADDRESS_LENGTH>;
    using AuthenticationKey = std::array<uint8_t, ADDRESS_LENGTH * 2>;
    using PublicKey = std::array<uint8_t, ADDRESS_LENGTH * 2>;
    using VecU8 = std::vector<uint8_t>;

    struct AddressAndIndex
    {
        Address address;
        size_t index;
    };

    enum AccountStatus
    {
        Local,
        Persisted,
        Unknow
    };

    struct Account
    {
        uint64_t index;
        Address address;
        AuthenticationKey auth_key;
        PublicKey pub_key;
        uint64_t sequence_number;
        AccountStatus status;

        bool operator==(const Account &right)
        {
            return this->index == right.index;
        }
    };

    struct TypeTag
    {
        Address address;
        std::string module_name;
        std::string resource_name;

        TypeTag(Address addr,
                std::string_view mod,
                std::string_view res)
            : address(addr),
              module_name(mod),
              resource_name(res)
        {
        }

        TypeTag(const TypeTag &tag)
            : address(tag.address),
              module_name(tag.module_name),
              resource_name(tag.resource_name)
        {
        }

        TypeTag(TypeTag &&tag)
        {
            address = std::move(tag.address);
            module_name = std::move(tag.module_name);
            resource_name = std::move(tag.resource_name);
        }

        TypeTag &operator=(TypeTag &&tag)
        {
            address = std::move(tag.address);
            module_name = std::move(tag.module_name);
            resource_name = std::move(tag.resource_name);

            return *this;
        }
    };

    const uint64_t MICRO_COIN = 1E+6;
    const size_t ASSOCIATION_ID = std::numeric_limits<size_t>::max();
    const size_t VIOLAS_ROOT_ACCOUNT_ID = std::numeric_limits<size_t>::max();
    const size_t VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID = VIOLAS_ROOT_ACCOUNT_ID - 1;
    const size_t VIOLAS_TESTNET_DD_ACCOUNT_ID = VIOLAS_ROOT_ACCOUNT_ID - 2;

    const Address ASSOCIATION_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x0A, 0x55, 0x0C, 0x18};
    const Address TESTNET_DD_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xDD};
    const Address CORE_CODE_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x01};

    inline TypeTag make_currency_tag(std::string_view currency_code)
    {
        return TypeTag(CORE_CODE_ADDRESS, currency_code, currency_code);
    }

    class Client
    {
    public:
        static std::shared_ptr<Client>
        create(uint8_t chain_id,
               std::string_view url,
               std::string_view mint_key,
               std::string_view mnemonic,
               std::string_view waypoint);

        virtual ~Client(){};

        virtual void
        test_connection() = 0;

        ////////////////////////////////////////////////////////////////
        //  wallet methods
        ////////////////////////////////////////////////////////////////
        virtual AddressAndIndex
        create_next_account(const std::optional<Address> &address = std::nullopt) = 0;

        virtual std::vector<Account>
        get_all_accounts() = 0;

        virtual void
        create_validator_account(std::string_view currency_code,
                                 const AuthenticationKey &auth_key,
                                 std::string_view human_name) = 0;

        virtual void
        mint_for_testnet(std::string_view currency_code,
                         const Address &receiver,
                         uint64_t amount) = 0;
        /**
         * @brief transfer currency
         * 
         * @param sender_account_ref_id the account index of client's wallet 
         * @param receiver_address      the address of receiver
         * @param currency_code         currency code
         * @param amount                the amount of currency
         * @param gas_unit_price        the gas unit price
         * @param max_gas_amount        the max gas amount, default is 1,000,000
         * @param gas_currency_code     the gas currency code. default is 'LBR'
         */
        virtual void
        transfer(size_t sender_account_ref_id,
                 const Address &receiver_address,
                 std::string_view currency_code,
                 uint64_t amount,
                 uint64_t gas_unit_price = 0,
                 uint64_t max_gas_amount = 1000000,
                 std::string_view gas_currency_code = "VLS") = 0;

        ////////////////////////////////////////////////////////////////
        //  management methods
        ////////////////////////////////////////////////////////////////

        //
        //  Allow  the custom scripts
        //  note that calling method needs violas root privilege
        virtual void
        allow_custom_script() = 0;

        //
        //  Allow  to publish custom module
        //  note that calling method needs violas root privilege
        virtual void
        allow_publishing_module(bool enabled) = 0;

        //
        //  publish a module file
        //  if account_index is ASSOCIATION_ID then publish module with association root account
        virtual void
        publish_module(size_t account_index,
                       std::string_view module_file_name) = 0;

        using TransactionAugment = std::variant<
            uint8_t,
            uint64_t,
            __uint128_t,
            Address,
            std::vector<uint8_t>,
            bool>;

        /**
         * @brief Execute script file with arguments
         * 
         * @param account_index account index of wallet
         * @param script_file_name script file name with path
         * @param type_tags transaction TypeTag vector for script
         * @param arguments transaction argument vector for script 
         */
        virtual void
        execute_script_file(size_t account_index,
                            std::string_view script_file_name,
                            const std::vector<TypeTag> &type_tags = {},
                            const std::vector<TransactionAugment> &arguments = {},
                            uint64_t max_gas_amount = 1'000'000,
                            uint64_t gas_unit_price = 0,
                            std::string gas_currency_code = "VLS",
                            bool is_blocking = true) = 0;

        /**
         * @brief Execute script bytecode with arguments
         * 
         * @param account_index account index of wallet
         * @param script_file_name script file name with path
         * @param type_tags transaction TypeTag vector for script
         * @param arguments transaction argument vector for script 
         */
        virtual void
        execute_script(size_t account_index,
                       const std::vector<uint8_t> &script,
                       const std::vector<TypeTag> &type_tags = {},
                       const std::vector<TransactionAugment> &arguments = {},
                       uint64_t max_gas_amount = 1'000'000,
                       uint64_t gas_unit_price = 0,
                       std::string gas_currency_code = "VLS",
                       bool is_blocking = true) = 0;

        /**
         * @brief Query accout status infomation
         * 
         * @param address - the address of account
         * @return std::string 
         */
        virtual std::string
        query_account_info(const Address &address) = 0;

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
                               bool is_fetching_events) = 0;
        /**
         * @brief Query transaction inforamtion by range
         * 
         * @param start_version     start version 
         * @param limit             limit of range, amount of queried transaction
         * @param is_fetching_event whether fectching event or not
         * @return std::string  with json format
         */
        virtual std::string
        query_transaction_range(uint64_t start_version,
                                uint64_t limit,
                                bool is_fetching_events) = 0;
        /**
         * @brief evnet type
         * 
         */
        enum payment_event_type
        {
            sent,
            received,

        };

        /**
         * @brief Query events
         * 
         * @param address       the address of account
         * @param type          evnet type
         * @param start_version start version
         * @param limit         limit of rang, amount of queried events
         * @return std::string  with json format
         */
        virtual std::string
        query_payment_events(const Address &address,
                             payment_event_type type,
                             uint64_t start_sn,
                             uint64_t limit) = 0;

        enum currency_event_type
        {
            minted = 0,
            burned,
            preburned,
            cancelled_burn,
            updated_exchange_rate,
        };

        virtual std::string
        query_currency_events(std::string_view currency_code,
                              currency_event_type type,
                              uint64_t start_sn,
                              uint64_t limit) = 0;

        virtual std::string
        query_account_creation_events(uint64_t start_sn,
                                      uint64_t limit) = 0;
        
        ///////////////////////////////////////////////////////
        // multi-currency method
        ///////////////////////////////////////////////////////

        // Call this method with root association privilege
        virtual void publish_curency(std::string_view currency_code) = 0;

        // Register currency with association root account
        virtual void
        register_currency(std::string_view currency_code,
                          uint64_t exchange_rate_denom,
                          uint64_t exchange_rate_num,
                          bool is_synthetic,
                          uint64_t scaling_factor,
                          uint64_t fractional_part) = 0;

        /// add currency for the designated dealer account
        virtual void
        add_currency_for_designated_dealer(
            std::string_view currency_code,
            const Address &dd_address) = 0;

        /// Add a currency to current account
        virtual void
        add_currency(size_t account_index,
                     std::string_view currency_code) = 0;

        /// get the balance of currency for the account address
        virtual uint64_t
        get_currency_balance(const Address &address,
                             std::string_view currency_code) = 0;

        /// Get all currency info
        virtual std::string
        get_all_currency_info() = 0;

        /// mint currency for dd account
        virtual void
        mint_currency_for_designated_dealer(std::string_view currency_code,
                                            uint64_t sliding_nonce,
                                            const Address &dd_address,
                                            uint64_t amount,
                                            uint64_t tier_index) = 0;

        virtual void
        create_designated_dealer_account(std::string_view currency_code,
                                         uint64_t nonce,
                                         const Address &new_account_address,
                                         const AuthenticationKey &auth_key,
                                         std::string_view human_name,
                                         std::string_view base_url,
                                         PublicKey compliance_public_key,
                                         bool add_all_currencies) = 0;

        virtual void
        create_designated_dealer_ex(std::string_view currency_code,
                                    uint64_t nonce,
                                    const Address &new_account_address,
                                    const AuthenticationKey &auth_key,
                                    std::string_view human_name,
                                    std::string_view base_url,
                                    PublicKey compliance_public_key,
                                    bool add_all_currencies) = 0;

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
                                   bool add_all_currencies) = 0;
        //
        // create child vasp account
        //
        virtual void
        create_child_vasp_account(std::string_view currency_code,
                                  size_t parent_account_index,
                                  const Address &new_account_address,
                                  const AuthenticationKey &auth_key,
                                  bool add_all_currencies,
                                  uint64_t initial_balance,
                                  bool is_blocking = true) = 0;

        /**
         * @brief rotate authentication key with nonce
         * 
         * @param account_index     account index
         * @param sliding_nonce     sliding nonce, default is 0
         * @param new_auth_key      the new authentication key 
         * @param is_blocking       if blocking and waiting for result
         */
        virtual void
        rotate_authentication_key_with_nonce(size_t account_index,
                                             uint64_t sliding_nonce,
                                             const AuthenticationKey &new_auth_key,
                                             bool is_blocking = true) = 0;

        /**
         * @brief recover account of wallet from specified 
         * 
         * @param mnemonic_file_path mnemonic file path
         */
        virtual void
        recover_wallet_accounts(std::string_view mnemonic_file_path) = 0;

        /**
         * @brief Save private key
         * 
         * @param account_index account index
         * @param path_file_str a file name string with path
         */
        virtual void
        save_private_key(size_t account_index, std::string_view path_file_str) = 0;

        /**
         * @brief Update daul attestation limit
         * 
         * @param sliding_nonce         sliding nonce
         * @param new_micro_lbr_limit   the new limit based micro LBR amount 
         */
        virtual void update_dual_attestation_limit(uint64_t sliding_nonce, uint64_t new_micro_lbr_limit) = 0;

        //  Exchnage interface
        //
        virtual std::string
        get_exchange_currencies(const Address &address) = 0;

        //
        //  get exchange reservers
        //
        virtual std::string
        get_exchange_reserves(const Address &address) = 0;

        //
        //  get liquidity balance
        //
        virtual std::string
        get_liquidity_balance(const Address &address) = 0;

        /**
         * @brief preburn
         * 
         * @param currency_code 
         * @param account_index 
         * @param amount 
         */
        virtual void
        preburn(std::string_view currency_code, size_t account_index, uint64_t amount, bool is_blocking) = 0;

        /**
         * @brief Permanently destroy the `Token`s stored in the oldest burn request under the `Preburn` resource
         * 
         * @param currency_code 
         * @param sliding_nonce 
         * @param preburn_address 
         */
        virtual void
        burn(std::string_view currency_code, uint64_t sliding_nonce, const Address &preburn_address, bool is_blocking) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
} //namespace violas

#endif