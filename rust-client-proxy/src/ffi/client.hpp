#ifndef VIOLAS_CLIENT
#define VIOLAS_CLIENT

#include <array>

using Address = std::array<uint8_t, 16>;

class Client
{
    void *internal;

public:
    Client();
    ~Client();

    void test_connection();

    struct Account
    {
        uint64_t index;
        Address address;
    }

    Account create_next_account();
};

#endif VIOLAS_CLIENT