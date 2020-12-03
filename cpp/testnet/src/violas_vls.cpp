#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <functional>
#include <client.hpp>
#include "utils.h"

using namespace std;
using namespace violas;

void initialize_timestamp(client_ptr client);
void mine_vls(client_ptr client);
void distribute_vls_to_bank_admin(client_ptr client);

const tuple<Address, string> VLS_ADDRESSES[] =
    // 0000000000000000000000000000DD00
    {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x00}, "garbage"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x01}, "community"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x02}, "investor"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x03}, "association"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x04}, "dev-team"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x05}, "consultant"},
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x06}, "contributor"}};

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

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { mine_vls(client); }},
            {2, [=]() { initialize_timestamp(client); }},
            {3, [=]() { distribute_vls_to_bank_admin(client); }},
        };

        cout << "1 for distribute vls \n"
                "2 for initialize vls timestamp \n"
                "3 for distribuate vls to bank adminitrator\n"
                "Please input index : ";

        int index;
        cin >> index;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << color::RED
                  << "caught an exception : " << e.what()
                  << color::RESET
                  << endl;
    }

    return 0;
}

void mine_vls(client_ptr client)
{
    vector<uint8_t> mint_vls_bytecode = {
        161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 1, 7, 8, 22, 8, 30,
        16, 0, 0, 0, 1, 0, 0, 0, 0, 12, 76, 105, 98, 114, 97, 65, 99, 99, 111, 117,
        110, 116, 8, 109, 105, 110, 101, 95, 118, 108, 115, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 17, 0, 2};

    //string
    auto [address0, name] = VLS_ADDRESSES[0];
    client->create_next_account(address0);

    auto accounts = client->get_all_accounts();

    client->execute_script(0, mint_vls_bytecode);

    cout << color::GREEN << "succeeded to mine VLS." << color::RESET << endl;
}

void initialize_timestamp(client_ptr client)
{
    client->allow_publishing_module(true);
    client->allow_custom_script();

    cout << "allow custom script and  publishing module." << endl;

    vector<uint8_t> vls_initialize_timestamp = {
        161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 1, 7, 8, 25, 8, 33,
        16, 0, 0, 0, 1, 0, 0, 0, 0, 3, 86, 76, 83, 20, 105, 110, 105, 116, 105, 97, 108,
        105, 122, 101, 95, 116, 105, 109, 101, 115, 116, 97, 109, 112, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 17, 0, 2};

    for (const auto address : VLS_ADDRESSES)
    {
        client->create_next_account(get<0>(address));
    }

    auto accounts = client->get_all_accounts();

    try_catch([&]() {
        for (auto account : accounts)
        {
            client->create_designated_dealer_ex("Coin1", 0,
                                                account.address, account.auth_key,
                                                get<1>(VLS_ADDRESSES[account.index]),
                                                "wwww.violas.io",
                                                account.pub_key, true);

            cout << "address : " << account.address
                 << ", auth key : " << account.auth_key << endl;
        }

        cout << color::GREEN << "Created all accounts for VLS receivers." << color::RESET << endl;
    });

    client->execute_script(0, vls_initialize_timestamp);

    cout << color::GREEN << "Initialized timestamp for VLS module." << color::RESET << endl;
}

void distribute_vls_to_bank_admin(client_ptr client)
{
    VecU8 set_incentive_rate = {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 15, 5, 17, 15,
                                7, 32, 51, 8, 83, 16, 6, 99, 5, 0, 0, 0, 1, 0, 1, 0, 0, 2, 2,
                                3, 0, 0, 3, 4, 3, 0, 1, 6, 12, 1, 1, 2, 6, 12, 10, 2, 0, 2, 6,
                                12, 3, 10, 86, 105, 111, 108, 97, 115, 66, 97, 110, 107, 12, 105,
                                115, 95, 112, 117, 98, 108, 105, 115, 104, 101, 100, 7, 112, 117,
                                98, 108, 105, 115, 104, 18, 115, 101, 116, 95, 105, 110, 99, 101,
                                110, 116, 105, 118, 101, 95, 114, 97, 116, 101, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 2, 2, 1, 0, 0, 4, 3, 13, 10, 0,
                                17, 0, 9, 33, 3, 6, 5, 9, 10, 0, 7, 0, 17, 1, 11, 0, 10, 1, 17, 2, 2};

    for (auto [address, name] : VLS_ADDRESSES)
    {
        client->create_next_account(address);
    }

    auto accounts = client->get_all_accounts();

    auto vls_balance = client->get_currency_balance(accounts[1].address, "VLS");
    cout << "account 1's VLS balance : " << (double)vls_balance / MICRO_COIN << endl;

    uint64_t amount = 0;
    cout << "pleaes input amount for distributing VLS to Bank admi : ";
    cin >> amount;
    cout << endl;

    client->execute_script(1, set_incentive_rate, {}, {amount * MICRO_COIN});
    cout << "distribute " << amount << " VLS to Bank admin" << endl;
}