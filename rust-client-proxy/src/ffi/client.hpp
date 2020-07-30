#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#include <memory>
#include <array>
#include <vector>
#include <string_view>

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
    };

    //using client_ptr = std::shared_ptr<Client>;
} //namespace violas

#endif