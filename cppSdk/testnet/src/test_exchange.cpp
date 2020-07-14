#include <iostream>
#include <string_view>
#include <iterator>
#include <ctime>
#include <cmath>
#include <violas_sdk.hpp>
#include "terminal.h"

using namespace std;

void test_exchange(const string &url,
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

    auto exchange = Exchange::create(client, script_path);

    //
    //  deploy module Exchange under association account
    //
    try_catch([&]() {
        client->publish_module(ASSOCIATION_ID, exdep_module);

        client->publish_module(ASSOCIATION_ID, exchange_module);

        //  initialize module Exchange under association account
        client->execute_script(ASSOCIATION_ID, initialize);
    });

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
    }

    auto print_all_balance = [=, &currencies](const Address &addr) {
        auto fmt_balance = [=](string_view currency) -> string {
            TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

            auto balance = client->get_currency_balance(tag, addr);
            return is_valid_balance(balance) ? to_string((double)balance / MICRO_COIN) : "N/A";
        };

        cout << "LBR : " << double(client->get_balance(accounts[user1].address)) / MICRO_COIN << ", "
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

        client->mint_currency(USD, accounts[user0].auth_key, 1 * MICRO_COIN);
        client->mint_currency(EUR, accounts[user0].auth_key, 10 * MICRO_COIN);
        client->mint_currency(GBP, accounts[user0].auth_key, 20 * MICRO_COIN);

        client->mint_currency(USD, accounts[user1].auth_key, 1 * MICRO_COIN);

        //add liquidity for USD -> EUR
        client->execute_script_ex({USD, EUR},
                                  user0,
                                  add_liquidity,
                                  {to_string(1 * MICRO_COIN), to_string(4 * MICRO_COIN), "0", "0"});

        //add liquidity for EUR -> GBP
        client->execute_script_ex({EUR, GBP},
                                  user0,
                                  add_liquidity,
                                  {to_string(4 * MICRO_COIN), to_string(16 * MICRO_COIN), "0", "0"});

        auto currencies = exchange->get_currencies(ASSOCIATION_ADDRESS);
        //cout << currencies << endl;

        auto reserve = exchange->get_reserves(ASSOCIATION_ADDRESS);
        cout << reserve << endl;

        auto liquidity_balance = exchange->get_liquidity_balance(accounts[0].address);
        cout << "liquidity balance is :" << liquidity_balance << endl;

        try_catch([&]() {
            uint64_t remove_amout = ((uint64_t)1000); // * 321432

            //client->execute_script_ex({USD, EUR}, user0, remove_liquidity, {to_string(remove_amout), "0", "0"}); //321432 //1000000
        });

        print_all_balance(accounts[0].address);

        client->execute_script_ex({USD, GBP}, user1, swap_currency, {to_string(1 * MICRO_COIN), "0", "b\"000102\""});
        print_all_balance(accounts[0].address);
    }

    print_all_balance(accounts[0].address);
    print_all_balance(accounts[1].address);
}

void run_exchange(const string &url,
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
    //auto u1 = client->create_next_account(true);
    //auto u2 = client->create_next_account(true);

    auto currencies_info = client->get_currency_info();
    cout << currencies_info << endl;

    string currency_codes[] = {
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

    TypeTag
        LBR(CORE_CODE_ADDRESS, "LBR", "LBR"),
        USD(CORE_CODE_ADDRESS, "VLSUSD", "VLSUSD"),
        EUR(CORE_CODE_ADDRESS, "VLSEUR", "VLSEUR"),
        GBP(CORE_CODE_ADDRESS, "VLSGBP", "VLSGBP");

    auto print_all_balance = [&](const Address &addr) {
        auto fmt_balance = [=](string_view currency) -> string {
            TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

            auto balance = client->get_currency_balance(tag, addr);
            return is_valid_balance(balance) ? to_string((double)balance / MICRO_COIN) : "N/A";
        };

        cout << "LBR : " << double(client->get_balance(addr)) / MICRO_COIN << ", "
             << "USD : " << fmt_balance("VLSUSD") << ", "
             << "EUR : " << fmt_balance("VLSEUR") << "."
             << "GBP : " << fmt_balance("VLSGBP") << "."
             << endl;
    };

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << endl;

        client->mint_coins(account.index, 10 * MICRO_COIN);

        try_catch([&]() {
            for (const auto &currency_code : currency_codes)
            {
                TypeTag currency_tag(CORE_CODE_ADDRESS, currency_code, currency_code);

                client->add_currency(currency_tag, account.index);
                client->mint_currency(currency_tag, account.auth_key, 100 * MICRO_COIN);
            }
        });
    }

    print_all_balance(accounts[0].address);
    print_all_balance(accounts[1].address);

    //////////////////////////////////////////////////////////////////////////////////////////

    size_t user0 = 0, user1 = 1;

    const string script_path = "../../cppSdk/move/exchange/";
    auto exchange = Exchange::create(client, script_path);

    try_catch([&]() { exchange->deploy_with_association_account(); });

    try_catch([&]() {
        for (auto currency : currency_codes)
        {
            exchange->add_currency(currency);
        }
    });

    exchange->add_liquidity(user0, {currency_codes[0], 1 * MICRO_COIN, 0}, {currency_codes[1], 2 * MICRO_COIN, 0});

    exchange->add_liquidity(user0, {currency_codes[1], 2 * MICRO_COIN, 0}, {currency_codes[2], 4 * MICRO_COIN, 0});

    exchange->add_liquidity(user0, {currency_codes[2], 4 * MICRO_COIN, 0}, {currency_codes[3], 8 * MICRO_COIN, 0});

    //exchange->add_liquidity(user0, {currency_codes[3], 1 * MICRO_COIN, 0}, {currency_codes[4], 4 * MICRO_COIN, 0});

    auto currencies = exchange->get_currencies(ASSOCIATION_ADDRESS);
    copy(begin(currencies), end(currencies), ostream_iterator<string>(cout, ", "));
    cout << endl;

    auto reserve = exchange->get_reserves(ASSOCIATION_ADDRESS);
    cout << reserve << endl;

    auto liquidity_balance = exchange->get_liquidity_balance(accounts[user0].address);
    cout << "liquidity balance is :" << liquidity_balance << endl;
    
    exchange->swap(user1, accounts[user1].address, currency_codes[0], 1 * MICRO_COIN, currency_codes[2], 0);

    print_all_balance(accounts[0].address);
    print_all_balance(accounts[1].address);
}