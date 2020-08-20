#include <iostream>
#include <string>
#include "violas_sdk.hpp"
#include "terminal.h"

using namespace std;

void run_test_bank(const string &url,
                   const string &mint_key_file,
                   const string &mnemonic_file,
                   const string &waypoint,
                   uint8_t chain_id)
{
    using namespace Violas;

    const auto &faucet = ASSOCIATION_ADDRESS;

    cout << color::RED << "running test for libra sdk ..." << color::RESET << endl;

    auto client = Client::create(chain_id, url, mint_key_file, true, "", mnemonic_file, waypoint);

    client->test_validator_connection();
    client->enable_custom_script(true);

    cout << "succeed to test validator connection ." << endl;

    client->create_next_account(true);
    client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        cout << "Account index : " << account.index
             << ", address : " << account.address
             << ", Authentication Key : " << account.auth_key
             << endl;
    }

    size_t user0 = 0;
    size_t user1 = 1;

    string currency_codes[] = {
        "VLSUSD",
        "VLSEUR",
        "VLSGBP",
        "VLSSGD",
        // "VLS",
        // "USD",
        // "EUR",
        // "GBP",
        // "SGD",
        // "BTC",
    };

    auto bank = Bank::create_bank(client, "../../move/bank/");
    //
    //  initialize
    //
    try_catch([=]() {
        //bank->deploy_with_association_account();
        cout << "deployed bank module successfully." << endl;

        bank->publish(0);   //BANK_ADMINISTRATOR_ID

        bank->publish(user0);
        bank->publish(user1);

        cout << "published successfully." << endl;

        for (auto currency_code : currency_codes)
        {
            bank->add_currency(currency_code,
                               ASSOCIATION_ADDRESS,
                               Bank::MANTISSA_1_0 / 2,       // 50%
                               Bank::MANTISSA_1_0 / 20,      // 5%
                               Bank::MANTISSA_1_0 / 10,      // 10%
                               Bank::MANTISSA_1_0 / 2,       // 50%
                               Bank::MANTISSA_1_0 * 8 / 10); // 80%
        }
    });

    cout << "Initialized data for Bank module" << endl;

    // try_catch([=]() {
    // });

    // bank->update_currency_price(currency_codes[0], Bank::MANTISSA_1_0 / 10);
    // bank->update_currency_price(currency_codes[1], Bank::MANTISSA_1_0 / 10);
    // bank->update_currency_price(currency_codes[2], Bank::MANTISSA_1_0 / 10);

    for (auto currency_code : currency_codes)
    {
        bank->update_currency_price(currency_code, Bank::MANTISSA_1_0 / 10);

        bank->enter(user0, currency_code, 100 * MICRO_COIN);
        bank->enter(user1, currency_code, 100 * MICRO_COIN);

        cout << "update for Bank module" << endl;
    }

    //  lock currency 0 with 10 MICRO_COIN
    bank->lock(user0, currency_codes[0], 10 * MICRO_COIN);

    //  lock currency 1 with 20 MICRO_COIN
    bank->lock(user0, currency_codes[1], 20 * MICRO_COIN);

    //  redeem currency 1 wiht 10 MICRO_CION
    bank->redeem(user0, currency_codes[1], 10 * MICRO_COIN);

    bank->lock(user1, currency_codes[2], 20 * MICRO_COIN);

    // borrow currency 2 with 10
    bank->borrow(user0, currency_codes[2], 10 * MICRO_COIN);

    // repay_borrow currency 2 with 10
    //bank->repay_borrow(user0, currency_codes[2], 10 * MICRO_COIN);

    bank->update_currency_price(currency_codes[2], Bank::MANTISSA_1_0 / 5); //20%

    // bank->enter(user1, "VLSEUR", 10 * MICRO_COIN);

    bank->liquidate_borrow(user1, currency_codes[2], accounts[user0].address, 9 * MICRO_COIN, currency_codes[0]);
}