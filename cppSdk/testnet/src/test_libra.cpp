#include <iostream>
#include <fstream>
#include <string>
#define LIB_NAME Libra
#include <violas_sdk.hpp>
#include <cassert>
#include "terminal.h"

using namespace std;

void run_test_libra(
    const string &host,
    uint16_t port,
    const string &mnemonic_file,
    const string &mint_key_file)
{
    using namespace Libra;

    cout << color::RED << "running test for libra sdk ..." << color::RESET << endl;

    auto client = Client::create(host, port, "", mint_key_file, true, "", mnemonic_file);

    client->test_validator_connection();
    cout << "succeed to test validator connection ." << endl;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << endl;
    }

    client->mint_coins(0, 10);
    client->mint_coins(1, 10);
    client->mint_coins(2, 10);
    client->mint_coins(3, 10);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    cout << "Transfer 1 libra coin from account 0 to account 1 ..." << endl;
    client->transfer_coins_int(0, accounts[1].address, 1 * MICRO_LIBRO_COIN);
    cout << "account 0' balance is " << client->get_balance(0) << endl
         << "account 1' balance is " << client->get_balance(1) << endl;

    auto print_txn = [client](Address address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    replace_mv_with_addr("../../cppSdk/scripts/violas.mv",
                         "violas.mv",
                         accounts[0].address);
                                        
    const auto faucet = Address::from_string("0000000000000000000000000A550C18");
    client->publish_module(0, "violas.mv");

    print_txn(faucet);
}