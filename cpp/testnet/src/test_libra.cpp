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
using namespace Libra;

client_ptr connect(const string &url, const string &mint_key_file, const string &mnemonic_file, const string &waypoint, uint8_t chain_id)
{

    cout << color::RED << "running test for libra sdk ..." << color::RESET << endl;
    cout << color::RED << "connecting to " << url << color::RESET << endl;

    auto client = Client::create(chain_id, url, mint_key_file, true, "", mnemonic_file, waypoint);

    client->test_validator_connection();
    cout << "succeed to test validator connection ." << endl;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);
    client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << ", Authentication Key : " << account.auth_key
             << endl;
    }

    //client->execute_script_json("", 0, {}, uint8_t(0), uint64_t(1), 2, true, ASSOCIATION_ADDRESS, vector<uint8_t>({0, 2, 3}));

    return client;
}

void run_test_libra(const string &url, const string &mint_key_file, const string &mnemonic_file, const string &waypoint, uint8_t chain_id)
{
    auto client = connect(url, mint_key_file, mnemonic_file, waypoint, chain_id);

    auto accounts = client->get_all_accounts();

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

    for (size_t i = 0; i < accounts.size() && i < 2; i++)
    {
        TypeTag LBR(CORE_CODE_ADDRESS, "LBR", "LBR");

        try_catch([&]() {
            client->create_testing_account(LBR, accounts[i].auth_key, true);
        });

        //client->mint_coins(i, 100);

        //client->mint_currency(TypeTag(Address(), "LBR", "T"), accounts[i].auth_key, 100 * MICRO_COIN);
        //client->mint_currency(TypeTag(Address(), "Coin1", "T"), accounts[i].auth_key, 100 * MICRO_COIN);
    }

    // print_account_balance(0);
    // print_account_balance(1);
    // client->transfer_coins_int(0, accounts[1].address, 100000);
    // print_account_balance(0);
    // print_account_balance(1);

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
    client->enable_custom_script(true);
    cout << "enable publsih custmer script." << endl;

    auto currencies = {
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

    for (const auto &currency : currencies)
    {
        TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

        try_catch([&]() {
            client->publish_currency(currency);
            cout << "publish currency : " << color::GREEN << currency << color::RESET << endl;
        });

        try_catch([&]() {
            client->register_currency(tag, 1, 2, false, 1000000, 100, currency);
            cout << "register currency : " << color::GREEN << currency << color::RESET << endl;
        });

        try_catch([&]() {
            client->add_currency_for_designated_dealer(tag, TESTNET_DD_ADDRESS);
            cout << "added currency "
                 << color::GREEN << currency << color::RESET
                 << " to account testnet dd account" << endl;
        });

        uint64_t sliding_nonce = 0;
        uint64_t tiered_index = 3;
        client->mint_currency(tag, sliding_nonce, TESTNET_DD_ADDRESS, 1000000 * MICRO_COIN, tiered_index);
        cout << "mint 1,000,000 currency "
             << color::GREEN << currency << color::RESET
             << " to account 0" << endl;

        try_catch([&]() {
            client->add_currency(tag, 0);
        });
        cout << "added currency " << currency << " to account 0" << endl;

        try_catch([&]() {
            client->add_currency(tag, 1);
        });
        cout << "added currency " << currency << " to account 1" << endl;

        //client->transfer_currency(tag, 0, accounts[1].address, 1000 * MICRO_COIN);
        cout << "transfer 500  currency " << currency << " from account 0 to account 1" << endl;

        print_txn(accounts[0].address);

        auto balance = client->get_currency_balance(tag, accounts[0].address);
        cout << "Account 0's balance for currency '" << currency << "' is " << (is_valid_balance(balance) ? to_string(balance) : "N/A") << endl;
        balance = client->get_currency_balance(tag, accounts[1].address);
        cout << "Account 1's balance for currency '" << currency << "' is " << (is_valid_balance(balance) ? to_string(balance) : "N/A") << endl;
    }

    auto info = client->get_currency_info();
    cout << endl
         << "All currencies info : " << info
         << endl;

    auto [state, version] = client->get_account_state(accounts[0].address);
    cout << "Account 0' state : " << state << endl;

    tie(events, last_status) = client->get_events(0, Client::EventType::sent, 0, 100);
    cout << "account 0's sent events : " << endl;
    copy(begin(events), end(events), ostream_iterator<string>(cout, "\n"));
    cout << last_status << endl;
}

void run_account_management(const string &url,
                            const string &mint_key_file,
                            const string &mnemonic_file,
                            const string &waypoint,
                            uint8_t chain_id)
{
    auto client = connect(url, mint_key_file, mnemonic_file, waypoint, chain_id);

    auto accounts = client->get_all_accounts();
    assert(accounts.size() >= 5);

    string currency = "VLSUSD";
    TypeTag tag(CORE_CODE_ADDRESS, currency, currency);
    TypeTag LBR(CORE_CODE_ADDRESS, "LBR", "LBR");

    auto pubkey = Bytes<32>::from_string("b7a3c12dc0c8c748ab07525b701122b88bd78f600c76342d27f25e5f92444cde");

    try_catch([&]() {
        client->create_parent_vasp_account(tag,
                                           accounts[2].auth_key,
                                           "Test3", "www.huntersun.me",
                                           pubkey.data().data(),
                                           true);
    });
    cout << "create VASP account for account 2" << endl;

    uint64_t sliding_nonce = 0;
    uint64_t tiered_index = 0;
    client->mint_currency(tag, sliding_nonce, accounts[2].address, 10 * MICRO_COIN, tiered_index);

    //client->mint_currency(LBR, accounts[2].auth_key, 10 * MICRO_COIN);

    auto balance = client->get_currency_balance(tag, accounts[2].address);

    try_catch([&]() {
        client->create_child_vasp_account(tag, 2, accounts[3].auth_key, true, 1 * MICRO_COIN);
    });

    cout << "Create child VASP account for account 3" << endl;

    auto [state, version] = client->get_account_state(accounts[3].address);
    cout << "Child Account 3's state is " << state << ", at version : " << version << endl;

    client->create_designated_dealer_account(tag, accounts[4].auth_key, 0);
    tie(state, version) = client->get_account_state(accounts[4].address);
    cout << "Treasury Compliance Account 4's state is " << state << ", at version : " << version << endl;
}