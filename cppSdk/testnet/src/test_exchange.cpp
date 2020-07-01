#include <iostream>
#include <string_view>
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

    uint64_t user0 = 0, user1 = 1;
    client->mint_coins(user0, 10 * MICRO_COIN);
    client->mint_coins(user1, 10 * MICRO_COIN);

    //client->mint_currency(TypeTag(CORE_CODE_ADDRESS, "LBR", "LBR"), accounts[0].auth_key, 10 * MICRO_COIN);

    auto print_txn = [client](const Address &address) {
        auto seq_num = client->get_sequence_number(address) - 1;
        auto txn = client->get_committed_txn_by_acc_seq(address, seq_num);
        cout << "txn = " << txn << endl;
    };

    const string script_path = "../../cppSdk/move/exchange/";
    const string exchange_module = script_path + "exchange.mv";
    const string exdep_module = script_path + "exdep.mv";
    const string initialize = script_path + "initialize.mv";
    const string add_currency = script_path + "add_currency.mv";
    const string add_liquidity = script_path + "add_liquidity.mv";
    const string remove_liquidity = script_path + "remove_liquidity.mv";
    const string swap_currency = script_path + "swap.mv";

    string currencies[] = {
        "VLSUSD", "VLSEUR", "VLSGBP", "VLSJPY", "VLSSGD",
        //"LBRUSD", "LBREUR", "LBRGBP", "LBRJPY", "LBRSGD",
        //"BTCBTC",
    };

    auto exchange = Exchange::create(client);

    //
    //  deploy module Exchange under association account
    //
    try_catch([&]() {
        client->publish_module(ASSOCIATION_ID, exdep_module);

        client->publish_module(ASSOCIATION_ID, exchange_module);

        //  initialize module Exchange under association account
        client->execute_script(ASSOCIATION_ID, initialize);

        //
        //  register all currency to Exchange module under association account
        //

        for (const auto &currency : currencies)
        {
            TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

            try_catch([&]() {
                client->execute_script(tag, ASSOCIATION_ID, add_currency);
            });

            try_catch([&]() {
                client->add_currency(tag, user0);
                client->add_currency(tag, user1);
            });

            client->mint_currency(tag, accounts[user0].auth_key, 10 * MICRO_COIN);
            client->mint_currency(tag, accounts[user1].auth_key, 10 * MICRO_COIN);
        }
    });

    auto print_all_balance = [=, &currencies](const Address &addr) {
        auto fmt_balance = [=](string_view currency) -> string {
            TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

            auto balance = client->get_currency_balance(tag, addr);
            return is_valid_balance(balance) ? to_string((double)balance / MICRO_COIN) : "N/A";
        };

        cout << "All balance for User --- LBR balance is "
             << double(client->get_balance(accounts[user1].address)) / MICRO_COIN << ", "
             << "USD : " << fmt_balance(currencies[0]) << ", "
             << "EUR : " << fmt_balance(currencies[1]) << "."
             << endl;
    };

    print_all_balance(accounts[0].address);
    print_all_balance(accounts[1].address);

    std::time_t deadline = std::time(nullptr) + 1000;

    //for (const auto &currency : currencies)
    {
        TypeTag USD(CORE_CODE_ADDRESS, "VLSUSD", "VLSUSD");
        TypeTag EUR(CORE_CODE_ADDRESS, "VLSEUR", "VLSEUR");
        TypeTag GBP(CORE_CODE_ADDRESS, "VLSGBP", "VLSGBP");

        // client->execute_script_ex({USD, EUR},
        //                           user0,
        //                           add_liquidity,
        //                           //{"1", to_string(10 * MICRO_COIN), to_string(0 * MICRO_COIN), to_string(0 * MICRO_COIN)});
        //                           {"1000000", "321432", "0", "0"});

        // client->execute_script_ex({USD, GBP},
        //                           user0,
        //                           add_liquidity,
        //                           //{"1", to_string(10 * MICRO_COIN), to_string(0 * MICRO_COIN), to_string(0 * MICRO_COIN)});
        //                           {"1000000", "321432", "0", "0"});

        auto currencies = exchange->get_currencies(ASSOCIATION_ADDRESS);
        cout << currencies << endl;
        
        auto reserve = exchange->get_reserves(ASSOCIATION_ADDRESS);
        cout << reserve << endl;

        auto liquidity_balance = exchange->get_liquidity_balance(accounts[0].address);
        cout << "liquidity balance is :" << liquidity_balance << endl;
        
        uint64_t remove_amout = (uint64_t)1000000 * 321432 * 0.5;
        client->execute_script_ex({EUR, USD}, user0, remove_liquidity, {to_string(remove_amout), "0", "0"}); //321432 //1000000
    }

    print_all_balance(accounts[0].address);
}