#include <iostream>
#include <string>
#include <string_view>
#include <iterator>
#include <map>
#include <memory>
#include <client.hpp> //rust/client-proxy/ffi/client.hpp
#include <iomanip>
#include <functional>
#include <violas_sdk2.hpp>
#include "terminal.h"

using namespace std;
using namespace violas;

template <size_t N>
std::ostream &operator<<(std::ostream &os, const array<uint8_t, N> &bytes)
{
    for (auto v : bytes)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

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

void run_test_case(client_ptr client);
void initialize_all_currencies(client_ptr client);
void deploy_exchange(client_ptr client);
void deploy_bank(client_ptr client);

int main(int argc, const char *argv[])
{
    try
    {
        if (argc < 6)
            throw runtime_error("missing arguments. \n Usage : url mint_key mnemonic waypoint chain_id");

        uint8_t chain_id = stoi(argv[5]);
        string url = argv[1];
        string mint_key = argv[2];
        string mnemonic = argv[3];
        string waypoint = argv[4];

        auto client = Client::create(chain_id, url, mint_key, mnemonic, waypoint);

        client->test_connection();

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { initialize_all_currencies(client); }},
            {2, [=]() { run_test_case(client); }},
            {3, [=]() { deploy_exchange(client); }},
            {4, [=]() { deploy_bank(client); }},
        };

        cout << "1 for deploying all currencies \n"
                "2 for testing Account Management \n"
                "3 for deploying Exchange Contract.\n"
                "4 for deploying Bank Contract.\n"
                "Please input index : ";

        int index;
        cin >> index;

        client->allow_publishing_module(true);
        client->allow_custom_script();

        cout << "allow custom script and  publishing module." << endl;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an exception : " << e.what() << '\n';
    }

    return 0;
}

void initialize_all_currencies(client_ptr client)
{
    cout << color::RED << "initialize all currencies ..." << color::RESET << endl;

    auto accounts = client->get_all_accounts();

    for (auto currency_code : currency_codes)
    {
        cout << color::GREEN << currency_code << color::RESET << "\t: ";

        client->publish_curency(currency_code);
        cout << "published, ";

        client->register_currency(currency_code,
                                  1,
                                  2,
                                  false,
                                  1000000,
                                  100);
        cout << "registered, ";

        client->add_currency_for_designated_dealer(currency_code,
                                                   TESTNET_DD_ADDRESS);
        cout << "added currency for DD account, ";

        client->mint_currency_for_designated_dealer(currency_code,
                                                    0,
                                                    TESTNET_DD_ADDRESS,
                                                    1000000 * MICRO_COIN,
                                                    3);
        cout << "minted 1,000,000 coins to DD account " << endl;
    }
}

void run_test_case(client_ptr client)
{
    client->create_next_account(BANK_ADMIN_ADDRESS);
    client->create_next_account(EXCHANGE_ADMIN_ADDRESS);

    auto accounts = client->get_all_accounts();
    auto LBR = "LBR", Coin1 = "Coin1";

    //for (size_t i = 0; i < 3; i++)

    for (const auto &account : accounts)
    {
        cout << "Address : " << account.address
             << ", Auth Key :" << account.auth_key
             << ", Sequence Number : " << account.sequence_number
             << endl;
    }

    //client->transfer(0, accounts[1].address, LBR, 5 * MICRO_COIN, 1);
    //AuthenticationKey dummy_auth_key = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0, 0x0, 0x0, 0x0};
    PublicKey pub_key = {0x1a, 0xbb, 0x9e, 0x6f, 0xec, 0x3f, 0x18, 0x21, 0x0a, 0x3a, 0xa1, 0x1d, 0x00, 0x47, 0x5d, 0xee,
                         0x76, 0xeb, 0xa7, 0x93, 0x78, 0x72, 0x37, 0x40, 0xb3, 0x7a, 0x9a, 0x2d, 0x09, 0x74, 0x96, 0xba};

    client->create_designated_dealer_account("LBR",
                                             0,
                                             BANK_ADMIN_ADDRESS,
                                             accounts[0].auth_key, //only auth key prefix is applied
                                             "Bank Administrator",
                                             "www.violas.io",
                                             pub_key,
                                             true);

    client->update_account_authentication_key(BANK_ADMIN_ADDRESS, accounts[0].auth_key);

    client->add_currency(0, "USD");

    client->create_designated_dealer_account("LBR",
                                             0,
                                             EXCHANGE_ADMIN_ADDRESS,
                                             accounts[1].auth_key, //only auth key prefix is applied
                                             "Exchange Administrator",
                                             "www.violas.io",
                                             pub_key,
                                             true);

    client->update_account_authentication_key(EXCHANGE_ADMIN_ADDRESS, accounts[1].auth_key);

    client->add_currency(1, "USD");
}

void deploy_exchange(client_ptr client)
{
    cout << color::RED << "Deploy Exchange and initialize it ..." << color::RESET << endl;

    auto admin = client->create_next_account(EXCHANGE_ADMIN_ADDRESS);
    auto user1 = client->create_next_account();
    auto user2 = client->create_next_account();

    auto accounts = client->get_all_accounts();
    auto &admin_account = accounts[admin.index];
    auto &user1_account = accounts[user1.index];
    auto &user2_account = accounts[user2.index];
    for (auto &account : accounts)
    {
        cout << "address : " << account.address << endl;
    }

    try_catch([&]() {
        client->create_designated_dealer_account("LBR",
                                                 0,
                                                 EXCHANGE_ADMIN_ADDRESS,
                                                 admin_account.auth_key, //only auth key prefix is applied
                                                 "Exchange Administrator",
                                                 "www.violas.io",
                                                 admin_account.pub_key,
                                                 true);

        client->update_account_authentication_key(EXCHANGE_ADMIN_ADDRESS, admin_account.auth_key);

        client->create_parent_vasp_account("LBR",
                                           0,
                                           user1_account.address,
                                           user1_account.auth_key,
                                           "Exchange user1",
                                           "www.violas.io",
                                           user1_account.pub_key,
                                           true);

        client->create_parent_vasp_account("LBR",
                                           0,
                                           user2_account.address,
                                           user2_account.auth_key,
                                           "Exchange user2",
                                           "www.violas.io",
                                           user2_account.pub_key,
                                           true);

        for (auto currency_code : currency_codes)
        {
            client->add_currency(admin.index, currency_code);

            client->add_currency(user1.index, currency_code);
            client->mint_for_testnet(currency_code, user1_account.address, 1000 * MICRO_COIN);

            client->add_currency(user2.index, currency_code);
            client->mint_for_testnet(currency_code, user2_account.address, 1000 * MICRO_COIN);
        }

        cout << "created all accounts for Exchange" << endl;
    });

    auto print_all_balance = [&](const Address &addr) {
        auto fmt_balance = [=](string_view currency) -> string {
            TypeTag tag(CORE_CODE_ADDRESS, currency, currency);

            auto balance = 0; //client->get_currency_balance(tag, addr);
            //return is_valid_balance(balance) ? to_string((double)balance / MICRO_COIN) : "N/A";
            return to_string((double)balance / MICRO_COIN);
        };

        cout << "LBR : " // << double(client->get_balance(addr)) / MICRO_COIN << ", "
             << "USD : " << fmt_balance("VLSUSD") << ", "
             << "EUR : " << fmt_balance("VLSEUR") << "."
             << "GBP : " << fmt_balance("VLSGBP") << "."
             << endl;
    };

    //print_all_balance(accounts[0].address);
    //print_all_balance(accounts[1].address);

    //////////////////////////////////////////////////////////////////////////////////////////

    const string script_path = "../../move/exchange/";
    auto exchange = Exchange::create(client, script_path);

    try_catch([&]() {
        exchange->deploy_with_root_account();
        cout << "deploied Exchange contracts on Violas blockchain." << endl;

        exchange->initialize(admin);
        cout << "Initialize Exchange contracts with admin account." << endl;

        for (auto currency : currency_codes)
        {
            exchange->add_currency(currency);
        }
        cout << "add all currencies for Exchange" << endl;
    });

    exchange->add_liquidity(user1.index, {currency_codes[0], 1 * MICRO_COIN, 0}, {currency_codes[1], 2 * MICRO_COIN, 0});

    exchange->add_liquidity(user1.index, {currency_codes[1], 2 * MICRO_COIN, 0}, {currency_codes[2], 4 * MICRO_COIN, 0});

    exchange->add_liquidity(user1.index, {currency_codes[2], 4 * MICRO_COIN, 0}, {currency_codes[3], 8 * MICRO_COIN, 0});

    //exchange->add_liquidity(user0, {currency_codes[3], 1 * MICRO_COIN, 0}, {currency_codes[4], 4 * MICRO_COIN, 0});
    cout << "added liquidity with account 1 for Exchange" << endl;

    auto currencies = exchange->get_currencies();
    copy(begin(currencies), end(currencies), ostream_iterator<string>(cout, ", "));
    cout << endl;

    auto reserve = exchange->get_reserves();
    cout << reserve << endl;

    auto liquidity_balance = exchange->get_liquidity_balance();
    cout << "liquidity balance is :" << liquidity_balance << endl;

    exchange->swap(user1.index, user1.address, currency_codes[0], 1 * MICRO_COIN, currency_codes[2], 0);

    print_all_balance(user1.address);
    print_all_balance(user2.address);
}

void deploy_bank(client_ptr client)
{
    cout << color::RED << "Deploy Bank Contract and initialize it ..." << color::RESET << endl;

    auto admin = client->create_next_account(BANK_ADMIN_ADDRESS);
    auto user1 = client->create_next_account();
    auto user2 = client->create_next_account();

    auto accounts = client->get_all_accounts();
    auto &admin_account = accounts[admin.index];
    auto &user1_account = accounts[user1.index];
    auto &user2_account = accounts[user2.index];
    for (auto &account : accounts)
    {
        cout << "address : " << account.address << endl;
    }

    try_catch([&]() {
        client->create_designated_dealer_account("LBR",
                                                 0,
                                                 BANK_ADMIN_ADDRESS,
                                                 admin_account.auth_key, //only auth key prefix is applied
                                                 "Bank Administrator",
                                                 "www.violas.io",
                                                 admin_account.pub_key,
                                                 true);

        client->update_account_authentication_key(BANK_ADMIN_ADDRESS, admin_account.auth_key);

        client->create_parent_vasp_account("LBR",
                                           0,
                                           user1_account.address,
                                           user1_account.auth_key,
                                           "Bank user1",
                                           "www.violas.io",
                                           user1_account.pub_key,
                                           true);

        client->create_parent_vasp_account("LBR",
                                           0,
                                           user2_account.address,
                                           user2_account.auth_key,
                                           "Bank user2",
                                           "www.violas.io",
                                           user2_account.pub_key,
                                           true);

        for (auto currency_code : currency_codes)
        {
            client->add_currency(admin.index, currency_code);

            client->add_currency(user1.index, currency_code);
            client->mint_for_testnet(currency_code, user1_account.address, 1000 * MICRO_COIN);

            client->add_currency(user2.index, currency_code);
            client->mint_for_testnet(currency_code, user2_account.address, 1000 * MICRO_COIN);
        }

        cout << "created all accounts for Bank contract." << endl;
    });

    auto bank = Bank::create_bank(client, "../../move/bank/");  //../../move/bank/
    //
    //  initialize
    //
    try_catch([=]() {
        bank->deploy_with_root_account();
        cout << "deployed bank module successfully." << endl;

        bank->initialize(admin);

        bank->publish(user1.index);
        bank->publish(user2.index);

        cout << "published successfully." << endl;

        for (auto currency_code : currency_codes)
        {
            bank->add_currency(currency_code,
                               admin.address,
                               Bank::MANTISSA_1_0 / 2,       // 50%
                               Bank::MANTISSA_1_0 / 20,      // 5%
                               Bank::MANTISSA_1_0 / 10,      // 10%
                               Bank::MANTISSA_1_0 / 5,       // 50%
                               Bank::MANTISSA_1_0 * 8 / 10); // 80%
        }

        cout << "added currencies successfully." << endl;
    });

    cout << "finished Initialization for Bank module" << endl;

    // try_catch([=]() {
    // });

    // bank->update_currency_price(currency_codes[0], Bank::MANTISSA_1_0 / 10);
    // bank->update_currency_price(currency_codes[1], Bank::MANTISSA_1_0 / 10);
    // bank->update_currency_price(currency_codes[2], Bank::MANTISSA_1_0 / 10);
    string currencies[] = {"USD", "EUR", "GBP"};

    for (auto currency_code : currencies)
    {
        bank->update_currency_price(currency_code, Bank::MANTISSA_1_0 / 10);

        bank->enter(user1.index, currency_code, 1000 * MICRO_COIN);
        bank->enter(user2.index, currency_code, 1000 * MICRO_COIN);

        cout << "update for Bank module" << endl;
    }

    //  lock currency 0 with 10 MICRO_COIN
    bank->lock(user1.index, currencies[0], 100 * MICRO_COIN);

    //
    //  lock  200 and redeem 100 for currency 1
    //
    bank->lock(user1.index, currencies[1], 200 * MICRO_COIN);
    bank->redeem(user1.index, currencies[1], 100 * MICRO_COIN);
    cout << "finished to redeem currency." << endl;
    //
    // borrow  and repay 100 for currency 2
    //
    bank->borrow(user1.index, currencies[2], 100 * MICRO_COIN);
    bank->repay_borrow(user1.index, currencies[2], 100 * MICRO_COIN);
    cout << "finished to repay_borrow currency." << endl;

    bank->borrow(user1.index, currencies[2], 100 * MICRO_COIN);

    bank->update_currency_price(currencies[2], Bank::MANTISSA_1_0 / 5); //20%
    cout << "finished to update currency price." << endl;

    // bank->enter(user1, "VLSEUR", 10 * MICRO_COIN);

    bank->liquidate_borrow(user2.index, currencies[2], user1.address, 90 * MICRO_COIN, currencies[0]);
}