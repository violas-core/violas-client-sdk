#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#include <memory>
#include <array>
#include <vector>
#include <string_view>
#include <limits>
#include <optional>

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

    struct CurrencyTag
    {
        Address address;
        std::string module_name;
        std::string resource_name;

        CurrencyTag(Address addr,
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

        virtual AddressAndIndex
        create_next_account() = 0;

        virtual std::vector<Account>
        get_all_accounts() = 0;

        virtual void
        create_testnet_account(const CurrencyTag &type_tag,
                               const AuthenticationKey &auth_key) = 0;

        virtual void
        mint_for_testnet(const CurrencyTag &currency_tag,
                         const Address &receiver,
                         uint64_t amount) = 0;

        virtual void
        transfer(size_t sender_account_ref_id,
                 const Address &receiver_address,
                 const CurrencyTag &currency_tag,
                 uint64_t amount,
                 uint64_t gas_unit_price = 0,
                 uint64_t max_gas_amount = 1000000,
                 const CurrencyTag & gas_currency_tag = CurrencyTag(CORE_CODE_ADDRESS, "LBR", "LBR")) = 0;
    };

    using client_ptr = std::shared_ptr<Client>;
} //namespace violas

#endif