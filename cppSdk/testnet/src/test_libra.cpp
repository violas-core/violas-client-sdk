#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#define LIB_NAME Libra
#include <violas_sdk.hpp>
#include <cassert>
#include "terminal.h"

using namespace std;

void run_test_libra(
    const string &url,
    const string &mint_key_file,
    const string &mnemonic_file,
    const string &waypoint)
{
    using namespace Libra;

    cout << color::RED << "running test for libra sdk ..." << color::RESET << endl;

    auto client = Client::create(url, mint_key_file, true, "", mnemonic_file, waypoint);

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

    auto print_account_balance = [=](uint64_t account_index) {
        auto coin = double(client->get_balance(account_index)) / MICRO_COIN;
        cout << "account " << account_index << "' balance is " << coin << endl;
    };

    print_account_balance(0);
    print_account_balance(1);
    client->transfer_coins_int(0, accounts[1].address, 100000);
    print_account_balance(0);
    print_account_balance(1);

    //print account 0's all sent events
    auto [events, last_status] = client->get_events(0, Client::EventType::sent, 0, 100);
    cout << "account 0's sent events : " << endl;
    copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    cout << last_status << endl;

    //print account 1's all received events
    tie(events, last_status) = client->get_events(1, Client::EventType::received, 0, 100);
    cout << "account 1's received events : " << endl;
    copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    cout << last_status << endl;

    auto print_txn = [client](Address address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    const auto faucet = Address::from_string("0000000000000000000000000A550C18");

    replace_mv_with_addr("../../cppSdk/scripts/violas.mv",
                         "violas.mv",
                         faucet);

    client->publish_module_with_faucet_account("violas.mv");
    print_txn(faucet);

    replace_mv_with_addr("../../cppSdk/scripts/violas_initialize.mv",
                         "violas_initialize.mv",
                         faucet);

    client->execute_script_with_faucet_account("violas_initialize.mv");
    print_txn(faucet);
}