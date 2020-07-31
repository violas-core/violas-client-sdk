#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <client.hpp> //rust-client-proxy/ffi/client.hpp
#include <iomanip>
#include "terminal.h"

using namespace std;
using namespace violas;

void run_test_case(client_ptr client);

template <size_t N>
std::ostream &operator<<(std::ostream &os, const array<uint8_t, N> &bytes)
{
    for (auto v : bytes)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

int main(int argc, const char *argv[])
{
    try
    {
        if (argc < 6)
            throw runtime_error("missing arguments. \n Usage : url mint_key mnemonic waypoint chain_id");

        uint8_t chain_id = stoi(argv[5]);
        string url = argv[1];
        string mint_key = argv[2];
        string mnemonic = argv[3];
        string waypoint = argv[4];

        auto client = Client::create(chain_id, url, mint_key, mnemonic, waypoint);

        client->test_connection();

        for (size_t i = 0; i < 3; i++)
            client->create_next_account();

        run_test_case(client);
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an exception : " << e.what() << '\n';
    }

    return 0;
}

void initialize_testnet(client_ptr client)
{
    auto accounts = client->get_all_accounts();
}

void run_test_case(client_ptr client)
{
    auto accounts = client->get_all_accounts();

    for (const auto &account : accounts)
    {
        CurrencyTag tag(CORE_CODE_ADDRESS, "LBR", "LBR");

        try_catch([&]() {
            client->create_testnet_account(tag, account.auth_key);
        },
                  false);

        client->mint_for_testnet(tag, account.address, 100 * MICRO_COIN);

        cout << "Address : " << account.address
             << ", Auth Key :" << account.auth_key
             << ", Sequence Number : " << account.sequence_number
             << endl;
    }
}