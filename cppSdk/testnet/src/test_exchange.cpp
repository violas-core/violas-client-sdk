#include <iostream>
#include <ctime>
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

    auto print_txn = [client](const Address &address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto txn = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    const string script_path = "../../cppSdk/move/compiled/";
    const string module = script_path + "exchange.mv";
    const string initialize = script_path + "exchange_initialize.mv";
    const string publish_reserve = script_path + "exchange_publish_reserve.mv";
    const string add_liquidity = script_path + "exchange_add_liquidity.mv";
    const string remove_liquidity = script_path + "exchange_remove_liquidity.mv";
    const string swap_from_token_to_violas = script_path + "exchange_token_to_violas_swap.mv";
    const string swap_from_violas_to_token = script_path + "exchange_violas_to_token_swap.mv";
    const string swap_from_token_to_token = script_path + "exchange_token_to_token_swap.mv";

    //
    //  deploy module Exchange under association account
    //
    client->publish_module(ASSOCIATION_ID, module);
    //print_txn(ASSOCIATION_ADDRESS);
    //
    //  initialize module Exchange under association account
    //
    client->execute_script(ASSOCIATION_ID, initialize);
    //print_txn(ASSOCIATION_ADDRESS);
    //
    //  register all currency to Exchange module under association account
    //
    vector<TypeTag> currencys = {TypeTag(ASSOCIATION_ADDRESS, "USD", "T"),
                                 TypeTag(ASSOCIATION_ADDRESS, "EUR", "T")};

    for (const auto &currency : currencys)
    {
        client->execute_script(currency, ASSOCIATION_ID, publish_reserve);
        print_txn(ASSOCIATION_ADDRESS);
    }
    uint64_t user1 = 0, user2 = 1;

    auto print_all_balance = [=, &currencys](const Address &addr) {
        auto fmt_balance = [=](const TypeTag &tag) -> string {
            auto balance = client->get_currency_balance(tag, addr);
            return is_valid_balance(balance) ? to_string((double)balance / MICRO_COIN) : "N/A";
        };

        cout << "All balance for User --- LBR balance is "
             << double(client->get_balance(accounts[user1].address)) / MICRO_COIN << ", "
             << "USD : " << fmt_balance(currencys[0]) << ", "
             << "EUR : " << fmt_balance(currencys[1]) << "."
             << endl;
    };

    print_all_balance(accounts[0].address);
    print_all_balance(accounts[1].address);

    std::time_t deadline = std::time(nullptr) + 1000;

    for (const auto &currency : currencys)
    {
        client->execute_script(currency,
                               user1,
                               add_liquidity,
                               {"1", to_string(10 * MICRO_COIN), to_string(5 * MICRO_COIN), to_string(deadline)});
        print_txn(accounts[user1].address);

        //client->execute_script(currency, user1, remove_liquidity,        )
    }

    print_all_balance(accounts[0].address);
}