#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#if __cplusplus < 201703
#error complier must be than c++17
#endif

#include <memory>
#include <array>
#include <vector>
#include <string_view>
#include <limits>
#include <variant>

namespace violas
{
    const size_t ADDRESS_LENGTH = 16;
    using Address = std::array<uint8_t, ADDRESS_LENGTH>;
    using AuthenticationKey = std::array<uint8_t, ADDRESS_LENGTH * 2>;

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
        Address address;
        AuthenticationKey auth_key;
        uint64_t sequence_number;
        AccountStatus status;
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
    };

    const uint64_t MICRO_COIN = 1000000;
    const uint64_t ASSOCIATION_ID = std::numeric_limits<uint64_t>::max();
    const Address ASSOCIATION_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x0A, 0x55, 0x0C, 0x18};
    const Address TESTNET_DD_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xDD};
    const Address CORE_CODE_ADDRESS = {00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0x01};

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
        create_next_account() = 0;

        virtual std::vector<Account>
        get_all_accounts() = 0;

        virtual void
        create_testnet_account(std::string_view currency_code,
                               const AuthenticationKey &auth_key) = 0;

        virtual void
        mint_for_testnet(std::string_view currency_code,
                         const Address &receiver,
                         uint64_t amount) = 0;
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
                 std::string_view gas_currency_code = "LBR") = 0;

        ////////////////////////////////////////////////////////////////
        //  management methods
        ////////////////////////////////////////////////////////////////
        enum VMPublishingOption
        {
            locked,
            open,
            custom_script
        };
        //
        //  Modify VM publishing option
        //  note that calling method needs association privilege
        virtual void
        modify_VM_publishing_option(VMPublishingOption option) = 0;

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

        //
        //  Execute script file with specified arguments
        //
        virtual void
        execute_script(size_t account_index,
                       std::string_view module_file_name,
                       const std::vector<TypeTag> &type_tags,
                       const std::vector<TransactionAugment> &arguments) = 0;

        //
        //  Execute script file with specified arguments
        //
        template <typename... Args>
        void execute_script(size_t account_index,
                            std::string_view script_file_name,
                            const std::vector<TypeTag> &type_tags,
                            const Args &... args)
        {
            using namespace std;

            std::vector<TransactionAugment> txn_args;
            auto parse_arg = [&](const auto &arg) {
                txn_args.push_back(arg);
            };

            ((parse_arg(args)), ...);
            //(args.push_back(std::forward<TransactionAugment>(args)), ...);

            execute_script(account_index, script_file_name, type_tags, txn_args);
        }

        ///////////////////////////////////////////////////////
        // multi-currency method
        ///////////////////////////////////////////////////////

        // Call this method with root association privilege
        virtual void
        publish_curency(std::string_view currency_code) = 0;

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

        /// mint currency for dd account
        virtual void
        mint_currency_for_designated_dealer(
            std::string_view currency_code,
            uint64_t sliding_nonce,
            const Address &dd_address,
            uint64_t amount,
            uint64_t tier_index) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
} //namespace violas

#endif