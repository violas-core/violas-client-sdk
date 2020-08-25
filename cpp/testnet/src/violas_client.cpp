#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <client.hpp> //rust-client-proxy/ffi/client.hpp
#include <iomanip>
#include <functional>
#include "terminal.h"

using namespace std;
using namespace violas;

template <size_t N>
std::ostream &operator<<(std::ostream &os, const array<uint8_t, N> &bytes)
{
    for (auto v : bytes)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

auto currency_codes = {
    "VLSUSD",
    "VLSEUR",
    "VLSGBP",
    "VLSSGD",
    "VLS",
    "USD",
    "EUR",
    "GBP",
    "SGD",
    "BTC",
};

void run_test_case(client_ptr client);
void initialize_all_currencies(client_ptr client);

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

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { initialize_all_currencies(client); }},
            {2, [=]() { run_test_case(client); }},

        };

        cout << "input index\n"
                "1 for initialize all currencies \n"
                "2 for testing Exchange \n"
                //"3 for testing Account Management.\n"
                //"4 for testing Bank \n"
                "Please input index : ";

        int index;
        cin >> index;

        client->modify_VM_publishing_option(Client::PublishingOption::open);

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an exception : " << e.what() << '\n';
    }

    return 0;
}

void initialize_all_currencies(client_ptr client)
{
    cout << color::RED << "initialize all currencies ..." << color::RESET << endl;

    auto accounts = client->get_all_accounts();

    for (auto currency_code : currency_codes)
    {
        cout << color::GREEN << currency_code << color::RESET << "\t: ";

        client->publish_curency(currency_code);
        cout << "published, ";

        client->register_currency(currency_code,
                                  1,
                                  2,
                                  false,
                                  1000000,
                                  100);
        cout << "registered, ";

        client->add_currency_for_designated_dealer(currency_code,
                                                   TESTNET_DD_ADDRESS);
        cout << "added currency for DD account, ";

        client->mint_currency_for_designated_dealer(currency_code,
                                                    0,
                                                    TESTNET_DD_ADDRESS,
                                                    1000000 * MICRO_COIN,
                                                    3);
        cout << "minted 1,000,000 coins to DD account " << endl;
    }
}

void run_test_case(client_ptr client)
{
    auto accounts = client->get_all_accounts();
    auto LBR = "LBR", Coin1 = "Coin1";

    int i = 0;
    for (const auto &account : accounts)
    {
        try_catch([&]() {
            client->create_testnet_account(LBR, account.auth_key);
            for (auto currency_code : currency_codes)
            {
                client->add_currency(i, currency_code);

                client->mint_for_testnet(currency_code, account.address, 100 * MICRO_COIN);
            }
        },
                  false);

        cout << "Address : " << account.address
             << ", Auth Key :" << account.auth_key
             << ", Sequence Number : " << account.sequence_number
             << endl;
        i++;
    }

    //client->transfer(0, accounts[1].address, LBR, 5 * MICRO_COIN, 1);
    auto bank_addr = Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x62, 0x61, 0x6E, 0x6B});
    AuthenticationKey auth_key = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0, 0x0, 0x0, 0x0};
    PublicKey pub_key = {0x1a, 0xbb, 0x9e, 0x6f, 0xec, 0x3f, 0x18, 0x21, 0x0a, 0x3a, 0xa1, 0x1d, 0x00, 0x47, 0x5d, 0xee,
                        0x76, 0xeb, 0xa7, 0x93, 0x78, 0x72, 0x37, 0x40, 0xb3, 0x7a, 0x9a, 0x2d, 0x09, 0x74, 0x96, 0xba};

    // client->create_designated_dealer_account("LBR",
    //                                          0,
    //                                          bank_addr,
    //                                          auth_key,
    //                                          "Bank administrator",
    //                                          "www.violas.io",
    //                                          pub_key,
    //                                          true);

    // client->update_account_authentication_key(bank_addr);

    client->add_currency(BANK_ADMINISTRATOR_ID, "USD");
}