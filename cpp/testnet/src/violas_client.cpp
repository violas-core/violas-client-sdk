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
        cout << color::GREEN << currency_code << color::RESET << "\t: " ;
        
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

            client->add_currency(i, Coin1);
        },
                  false);

        client->mint_for_testnet(LBR, account.address, 100 * MICRO_COIN);

        client->mint_for_testnet(Coin1, account.address, 10 * MICRO_COIN);

        cout << "Address : " << account.address
             << ", Auth Key :" << account.auth_key
             << ", Sequence Number : " << account.sequence_number
             << endl;
        i++;
    }

    client->transfer(0, accounts[1].address, LBR, 5 * MICRO_COIN, 1);
}