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

template <typename F>
void try_catch(F f)
{
    try
    {
        f();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void run_test_libra(const string &url, const string &mint_key_file, const string &mnemonic_file, const string &waypoint)
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
             << ", Authentication Key : " << account.auth_key
             << endl;
    }

    auto print_txn = [client](Address address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto txn = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    const auto &faucet = ASSOCIATION_ADDRESS;

    auto print_account_balance = [=](uint64_t account_index) {
        auto coin = double(client->get_balance(account_index)) / MICRO_COIN;
        cout << "account " << account_index << "' balance is " << coin << endl;
    };

    for (size_t i = 0; i < accounts.size(); i++)
    {
        client->mint_coins(i, 100);

        //client->mint_currency(TypeTag(Address(), "LBR", "T"), accounts[i].auth_key, 100 * MICRO_COIN);
        //client->mint_currency(TypeTag(Address(), "Coin1", "T"), accounts[i].auth_key, 100 * MICRO_COIN);
    }

    print_account_balance(0);
    print_account_balance(1);
    client->transfer_coins_int(0, accounts[1].address, 100000);
    print_account_balance(0);
    print_account_balance(1);

    //print account 0's all sent events
    auto [events, last_status] = client->get_events(0, Client::EventType::sent, 0, 100);
    cout << "account 0's sent events : " << endl;
    //copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    //cout << last_status << endl;
    //
    // print account 1's all received events
    //
    tie(events, last_status) = client->get_events(1, Client::EventType::received, 0, 100);
    cout << "account 1's received events : " << endl;
    //copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    //cout << last_status << endl;

    ////////////////////////////////////////////////////////////////////////////////
    //  test multi currency for move language stdlib
    ////////////////////////////////////////////////////////////////////////////////

    //const auto root   = Address::from_string("00000000000000000000000000000000");

    auto args = {make_tuple("coin_usd.mv", "USD", u8"USD"),
                 make_tuple("coin_eur.mv", "EUR", u8"EUR"),
                 make_tuple("coin_gbp.mv", "GBP", u8"GBP")};

    for (const auto &arg : args)
    {
        auto contract = get<0>(arg);
        auto module_name = get<1>(arg);
        TypeTag tag(CORE_CODE_ADDRESS, module_name, "T");

        replace_mv_with_addr(string("../../cppSdk/move/compiled/") + contract,
                             contract,
                             faucet);

        try_catch([&]() {
            client->publish_module(ASSOCIATION_ID, contract);
        });
        cout << "publish module : " << module_name << endl;

        auto currency_code = get<2>(arg);
        try_catch([&]() {
            client->register_currency(tag, 1, 2, false, 1000000, 100, currency_code);
        });
        //client->register_currency(tag, 1, 2, false, 1000000, 100, currency_code);
        cout << "registered currency " << module_name << endl;
        //print_txn(ASSOCIATION_ADDRESS);

        try_catch([&]() {
            client->add_currency(tag, 0);
        });
        cout << "added currency " << module_name << " to account 0" << endl;

        try_catch([&]() {
            client->add_currency(tag, 1);
        });
        cout << "added currency " << module_name << " to account 1" << endl;

        client->mint_currency(tag, accounts[0].auth_key, 1000 * MICRO_COIN);
        cout << "mint 1000 currency " << module_name << " to account 0" << endl;

        client->transfer_currency(tag, 0, accounts[1].auth_key, 500 * MICRO_COIN);
        cout << "transfer 500  currency " << module_name << " from account 0 to account 1" << endl;

        print_txn(accounts[0].address);

        auto balance = client->get_currency_balance(tag, accounts[0].address);
        cout << "Account 0's balance for currency '" << currency_code << "' is " << (is_valid_balance(balance) ? to_string(balance) : "N/A") << endl;
        balance = client->get_currency_balance(tag, accounts[1].address);
        cout << "Account 1's balance for currency '" << currency_code << "' is " << (is_valid_balance(balance) ? to_string(balance) : "N/A") << endl;
    }

    auto info = client->get_currency_info();
    cout << endl
         << "All currencies info : " << info
         << endl;

    tie(events, last_status) = client->get_events(0, Client::EventType::sent, 0, 100);
    cout << "account 0's sent events : " << endl;
    copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    cout << last_status << endl;
}