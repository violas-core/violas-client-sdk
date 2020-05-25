#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <tuple>
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
    //
    // print account 1's all received events
    //
    tie(events, last_status) = client->get_events(1, Client::EventType::received, 0, 100);
    cout << "account 1's received events : " << endl;
    //copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    cout << last_status << endl;

    auto print_txn = [client](Address address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //  test multi currency for move language stdlib
    ////////////////////////////////////////////////////////////////////////////////

    //const auto root   = Address::from_string("00000000000000000000000000000000");
    const auto &faucet = ASSOCIATION_ADDRESS;

    auto args = {make_tuple("coin_usd.mv", "USD", u8"Coin1"),
                 make_tuple("coin_eur.mv", "EUR", u8"Coin2")};

    for (const auto &arg : args)
    {
        auto contract = get<0>(arg);
        replace_mv_with_addr(string("../../cppSdk/move/compiled/") + contract,
                             contract,
                             faucet);

        client->publish_module(ASSOCIATION_ID, contract);
        //client->publish_module(0, contract);
        print_txn(faucet);

        auto module_name = get<1>(arg);
        Client::TypeTag tag(faucet, module_name, "T");

        auto currency_code = get<2>(arg);
        client->add_currency(tag, 1, 2, false, 1000000, 100, currency_code);
        print_txn(faucet);

        client->register_currency(tag, 0);
        print_txn(accounts[0].address);

        client->mint_currency(tag, accounts[0].auth_key, 10);
        print_txn(faucet);

        client->register_currency(tag, 1);
        print_txn(accounts[1].address);

        client->transfer_currency(tag, 0, accounts[1].auth_key, 5);
        print_txn(accounts[0].address);

        auto balance = client->get_currency_balance(tag, accounts[1].address);
        cout << "balance is " << (is_valid_balance(balance) ? to_string(balance) : "N/A") << endl;
    }


}