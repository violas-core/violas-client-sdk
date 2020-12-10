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
void recover_vls_fees_to_association(client_ptr client);

const tuple<Address, string> VLS_ADDRESSES[] = {
    // 0000000000000000000000000000DD00
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x00}, "Trash"},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x01}, "VLS-COMM"},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x02}, "VLS-ASSOCA"},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x03}, "VLS-TEAM"},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x04}, "VLS-ADVS"},
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xDD, 0x05}, "VLS-OPEN"},
};

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
            {4, [=]() { recover_vls_fees_to_association(client); }},
        };

        cout << "1 for distribute vls \n"
                "2 for initialize vls timestamp \n"
                "3 for distribuate vls to all Violas adminitrators\n"
                "4 for recover vls fees toassociation\n"
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
    // VecU8 set_incentive_rate = {161, 28, 235, 11, 1, 0, 0, 0, 6, 1, 0, 2, 3, 2, 15, 5, 17, 15,
    //                             7, 32, 51, 8, 83, 16, 6, 99, 5, 0, 0, 0, 1, 0, 1, 0, 0, 2, 2,
    //                             3, 0, 0, 3, 4, 3, 0, 1, 6, 12, 1, 1, 2, 6, 12, 10, 2, 0, 2, 6,
    //                             12, 3, 10, 86, 105, 111, 108, 97, 115, 66, 97, 110, 107, 12, 105,
    //                             115, 95, 112, 117, 98, 108, 105, 115, 104, 101, 100, 7, 112, 117,
    //                             98, 108, 105, 115, 104, 18, 115, 101, 116, 95, 105, 110, 99, 101,
    //                             110, 116, 105, 118, 101, 95, 114, 97, 116, 101, 0, 0, 0, 0, 0, 0,
    //                             0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 2, 2, 1, 0, 0, 4, 3, 13, 10, 0,
    //                             17, 0, 9, 33, 3, 6, 5, 9, 10, 0, 7, 0, 17, 1, 11, 0, 10, 1, 17, 2, 2};

    static VecU8 script_bytecode = {
        161, 28, 235, 11, 1, 0, 0, 0, 8, 1, 0, 14, 2, 14, 16, 3, 30, 83, 4, 113, 6, 5, 119, 102, 7, 221, 1, 200, 2, 8, 165, 4, 16, 6, 181, 4, 75, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 1, 1, 2, 0, 2, 17, 1, 0, 4, 22, 2, 0, 4, 4, 2, 0, 0, 7, 0, 0, 0, 0, 8, 0, 0, 0, 1, 9, 1, 2, 0, 1, 10, 3, 0, 0, 1, 11, 3, 0, 0, 3, 12, 4, 5, 0, 5, 13, 6, 7, 1, 1, 6, 14, 4, 8, 0, 6, 15, 9, 10, 0, 6, 16, 11, 10, 0, 2, 18, 5, 0, 1, 1, 2, 19, 4, 12, 0, 2, 20, 13, 10, 1, 1, 2, 21, 12, 10, 0, 4, 23, 10, 14, 0, 4, 24, 15, 16, 0, 6, 15, 10, 18, 12, 18, 1, 3, 2, 3, 3, 1, 8, 0, 2, 3, 8, 0, 1, 6, 12, 1, 5, 2, 6, 10, 9, 0, 3, 1, 6, 9, 0, 1, 1, 2, 6, 12, 10, 2, 0, 2, 6, 12, 3, 1, 8, 1, 5, 6, 8, 1, 5, 3, 10, 2, 10, 2, 1, 10, 8, 2, 1, 8, 2, 2, 5, 8, 0, 23, 5, 8, 0, 3, 8, 0, 3, 3, 3, 3, 8, 0, 8, 1, 8, 0, 6, 8, 2, 10, 8, 2, 5, 7, 10, 8, 2, 3, 1, 3, 1, 3, 1, 3, 3, 1, 8, 3, 6, 69, 114, 114, 111, 114, 115, 12, 70, 105, 120, 101, 100, 80, 111, 105, 110, 116, 51, 50, 12, 76, 105, 98, 114, 97, 65, 99, 99, 111, 117, 110, 116, 6, 83, 105, 103, 110, 101, 114, 3, 86, 76, 83, 6, 86, 101, 99, 116, 111, 114, 10, 86, 105, 111, 108, 97, 115, 66, 97, 110, 107, 14, 108, 105, 109, 105, 116, 95, 101, 120, 99, 101, 101, 100, 101, 100, 16, 114, 101, 113, 117, 105, 114, 101, 115, 95, 97, 100, 100, 114, 101, 115, 115, 20, 99, 114, 101, 97, 116, 101, 95, 102, 114, 111, 109, 95, 114, 97, 116, 105, 111, 110, 97, 108, 10, 100, 105, 118, 105, 100, 101, 95, 117, 54, 52, 12, 109, 117, 108, 116, 105, 112, 108, 121, 95, 117, 54, 52, 10, 97, 100, 100, 114, 101, 115, 115, 95, 111, 102, 6, 98, 111, 114, 114, 111, 119, 12, 105, 115, 95, 112, 117, 98, 108, 105, 115, 104, 101, 100, 7, 112, 117, 98, 108, 105, 115, 104, 18, 115, 101, 116, 95, 105, 110, 99, 101, 110, 116, 105, 118, 101, 95, 114, 97, 116, 101, 18, 87, 105, 116, 104, 100, 114, 97, 119, 67, 97, 112, 97, 98, 105, 108, 105, 116, 121, 7, 98, 97, 108, 97, 110, 99, 101, 27, 101, 120, 116, 114, 97, 99, 116, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 8, 112, 97, 121, 95, 102, 114, 111, 109, 27, 114, 101, 115, 116, 111, 114, 101, 95, 119, 105, 116, 104, 100, 114, 97, 119, 95, 99, 97, 112, 97, 98, 105, 108, 105, 116, 121, 8, 82, 101, 99, 101, 105, 118, 101, 114, 13, 103, 101, 116, 95, 114, 101, 99, 101, 105, 118, 101, 114, 115, 15, 117, 110, 112, 97, 99, 107, 95, 114, 101, 99, 101, 105, 118, 101, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 8, 233, 3, 0, 0, 0, 0, 0, 0, 3, 8, 234, 3, 0, 0, 0, 0, 0, 0, 3, 8, 232, 3, 0, 0, 0, 0, 0, 0, 10, 2, 2, 1, 0, 5, 16, 117, 36, 241, 69, 193, 51, 180, 139, 107, 10, 212, 60, 77, 145, 114, 35, 10, 2, 1, 0, 5, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 221, 0, 0, 4, 17, 120, 10, 0, 17, 5, 12, 14, 17, 14, 12, 13, 13, 13, 12, 15, 11, 15, 46, 6, 0, 0, 0, 0, 0, 0, 0, 0, 56, 0, 12, 12, 11, 12, 20, 17, 15, 12, 11, 12, 1, 10, 14, 10, 1, 33, 7, 2, 17, 1, 12, 18, 12, 17, 11, 17, 3, 30, 11, 0, 1, 11, 18, 39, 10, 14, 56, 1, 12, 3, 10, 3, 11, 11, 17, 3, 12, 23, 6, 16, 0, 0, 0, 0, 0, 0, 0, 6, 100, 0, 0, 0, 0, 0, 0, 0, 17, 2, 12, 4, 6, 30, 0, 0, 0, 0, 0, 0, 0, 6, 100, 0, 0, 0, 0, 0, 0, 0, 17, 2, 12, 9, 6, 15, 0, 0, 0, 0, 0, 0, 0, 6, 100, 0, 0, 0, 0, 0, 0, 0, 17, 2, 12, 2, 10, 0, 17, 7, 9, 33, 3, 55, 5, 58, 10, 0, 7, 3, 17, 8, 10, 23, 11, 4, 17, 4, 12, 5, 10, 0, 10, 5, 17, 9, 10, 14, 56, 1, 10, 3, 10, 5, 23, 33, 7, 0, 17, 0, 12, 20, 12, 19, 11, 19, 3, 81, 11, 0, 1, 11, 20, 39, 10, 23, 11, 9, 17, 4, 12, 6, 10, 6, 6, 1, 0, 0, 0, 0, 0, 0, 0, 36, 12, 21, 11, 21, 3, 95, 11, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 39, 11, 0, 17, 11, 12, 10, 10, 23, 11, 2, 17, 4, 12, 7, 14, 10, 7, 4, 10, 7, 7, 5, 7, 5, 56, 2, 10, 14, 56, 1, 12, 8, 14, 10, 7, 6, 10, 8, 7, 5, 7, 5, 56, 2, 11, 10, 17, 13, 2};

    for (auto [address, name] : VLS_ADDRESSES)
    {
        client->create_next_account(address);
    }

    auto accounts = client->get_all_accounts();

    auto vls_balance = client->get_currency_balance(accounts[1].address, "VLS");
    cout << "account 1's VLS balance : " << (double)vls_balance / MICRO_COIN << endl;

    // uint64_t amount = 0;
    // cout << "pleaes input amount for distributing VLS to Bank administrator : ";
    // cin >> amount;
    // cout << endl;

    //client->execute_script(1, set_incentive_rate, {}, {amount * MICRO_COIN});

    client->execute_script(1, script_bytecode, {}, {});

    auto amount = vls_balance - client->get_currency_balance(accounts[1].address, "VLS");
    cout << "distribute " << amount << " VLS to Bank admin" << endl;
}

void recover_vls_fees_to_association(client_ptr client)
{
    static VecU8 script_bytecode = {
        161, 28, 235, 11, 1, 0, 0, 0, 5, 1, 0, 2, 3, 2, 5, 5, 7, 4, 7, 11, 45, 8, 56, 16, 0, 0, 0, 1, 0, 1, 0, 1, 6, 12, 0, 12, 76, 105, 98, 114, 97, 65, 99, 99, 111, 117, 110, 116, 31, 114, 101, 99, 111, 118, 101, 114, 95, 118, 108, 115, 95, 102, 101, 101, 115, 95, 116, 111, 95, 97, 115, 115, 111, 99, 105, 97, 116, 105, 111, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 11, 0, 17, 0, 2};

    for (auto [address, name] : VLS_ADDRESSES)
    {
        client->create_next_account(address);
    }

    auto accounts = client->get_all_accounts();
    const size_t ASSOCIATION_INDEX_ID = 2;

    auto vls_balance = client->get_currency_balance(accounts[ASSOCIATION_INDEX_ID].address, "VLS");
    cout << "Association account's VLS balance : " << vls_balance << endl;

    client->execute_script(VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID, script_bytecode, {}, {});

    vls_balance = client->get_currency_balance(accounts[ASSOCIATION_INDEX_ID].address, "VLS");
    cout << "Association account's VLS balance : " << vls_balance << endl;
}