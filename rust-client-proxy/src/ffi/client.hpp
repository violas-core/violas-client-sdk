#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#include <array>
#include <string_view>

using Address = std::array<uint8_t, 16>;

class Client
{
    void *internal;

public:
    Client(std::string_view url, std::string_view mint_key, std::string_view mnemonic);

    ~Client();

    void test_connection();

    struct Account
    {
        uint64_t index;
        Address address;
    };

    Account create_next_account();
};

#endif 