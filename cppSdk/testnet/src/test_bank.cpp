#include <iostream>
#include <string>
#include "violas_sdk.hpp"
#include "terminal.h"

using namespace std;

void run_test_bank(const string &url,
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

    auto user0 = client->create_next_account(true);
    auto user1 = client->create_next_account(true);

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

    auto bank = Bank::create_bank(client, "../../cppSdk/move/bank/");
    try_catch([=]() {
        bank->deploy_with_association_account();

        bank->publish(ASSOCIATION_ID);
        bank->publish(user0.first);
        bank->publish(user1.first);

        bank->add_currency(currency_codes[0], ASSOCIATION_ADDRESS, 2147483648);
        bank->add_currency(currency_codes[1], ASSOCIATION_ADDRESS, 2147483648);
        bank->add_currency(currency_codes[2], ASSOCIATION_ADDRESS, 2147483648);

        bank->update_currency_price(currency_codes[0], 429496729);
        bank->update_currency_price(currency_codes[1], 429496729);
        bank->update_currency_price(currency_codes[2], 429496729);

        cout << "Initialize data for Bank module" << endl;
    });

    bank->enter(user0.first, "VLSUSD", 10 * MICRO_COIN);
    bank->lock(user0.first, "VLSUSD", 2 * MICRO_COIN);

    bank->enter(user1.first, "VLSEUR", 10 * MICRO_COIN);
    bank->lock(user1.first, "VLSEUR", 2 * MICRO_COIN);

    bank->borrow(user1.first, "VLSUSD", 1 * MICRO_COIN);    
}