#include <iostream>
#include <violas_sdk.hpp>
#include "terminal.h"

using namespace std;

void run_test_exchange(const string &url,
                       const string &mint_key_file,
                       const string &mnemonic_file,
                       const string &waypoint)
{
    using namespace Violas;

    const auto &faucet = ASSOCIATION_ADDRESS;

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

    auto print_txn = [client](const Address &address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    const string script_path = "../../cppSdk/move/compiled/";
    const auto module = script_path + "exchange.mv";
    const auto initialize = script_path + "exchange_intialize.mv";

    client->publish_module(ASSOCIATION_ID, module);
    print_txn(ASSOCIATION_ADDRESS);

    client->execute_script(ASSOCIATION_ID, initialize);
    print_txn(ASSOCIATION_ADDRESS);

}