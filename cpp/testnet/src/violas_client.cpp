#include <iostream>
#include <string>
#include <string_view>
#include <iterator>
#include <map>
#include <tuple>
#include <memory>
#include <client.hpp> //rust/client-proxy/ffi/client.hpp
#include <iomanip>
#include <functional>
#include <violas_sdk2.hpp>
#include "utils.h"

using namespace std;
using namespace violas;

string currency_codes[] = {
    "VLSUSD",
    "VLSEUR",
    "VLSGBP",
    "VLSSGD",
    "USD",
    "EUR",
    "GBP",
    "SGD",
    "BTC",
    "USDT",
};

enum account_type
{
    DD,
    VASP,
    CHILD_VASP,
};

void update_dual_attestation_limit(client_ptr client);
void initialize_all_currencies(client_ptr client);
void deploy_exchange(client_ptr client);
void deploy_bank(client_ptr client);
void mint_currency(client_ptr client);
void register_currency(client_ptr client);
void create_bridge_accounts(client_ptr client);

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

        cout << "The current used mnemonic file is "
             << color::RED << mnemonic << color::RESET
             << "."
             << endl;

        using handler = function<void()>;
        map<int, handler> handlers = {
            {0, [=]() { register_currency(client); }},
            {1, [=]() { initialize_all_currencies(client); }},
            {2, [=]() { update_dual_attestation_limit(client); }},
            {3, [=]() { deploy_exchange(client); }},
            {4, [=]() { deploy_bank(client); }},
            {5, [=]() { mint_currency(client); }},
            {6, [=]() { create_bridge_accounts(client); }},
        };

        cout << "1 for deploying all currencies \n"
                "2 for updating dual attestation limit \n"
                "3 for deploying Exchange Contract.\n"
                "4 for deploying Bank Contract.\n"
                "5 for minting curreny to DD account.\n"
                "6 for creating all accounts for Bridge service.\n"
                "7 for rotate authentication key.\n"
                "Please input index : ";

        int index;
        cin >> index;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an exception : " << e.what() << '\n';
    }

    return 0;
}

void check_password()
{
    string pwd;
    hash<string> hash_str;

    set_stdin_echo(false); // disable echoing password
    cout << "please input password :";
    cin >> pwd;
    set_stdin_echo(true);

    if (hash_str(pwd) != 12375900582722818748U)
        throw runtime_error("password is incorrect");

    cout << "\nPassword was verified successfully." << endl;
}

void initialize_all_currencies(client_ptr client)
{
    client->allow_publishing_module(true);
    client->allow_custom_script();

    cout << "allow custom script and  publishing module." << endl;

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
                                                    1'000'000 * MICRO_COIN,
                                                    3);
        cout << "minted 1,000,000 coins to DD account " << endl;
    }

    cout << "all currency info : " << client->get_all_currency_info() << endl;
}
/**
 * @brief Register currency
 * 
 * @param client 
 */
void register_currency(client_ptr client)
{
    cout << color::GREEN << "Register a new currency ..." << color::RESET << endl;

    string pwd;
    hash<string> hash_str;

    set_stdin_echo(false); // disable echoing password
    cout << "please input password for registering currency : ";
    cin >> pwd;
    set_stdin_echo(true);

    if (hash_str(pwd) != 12375900582722818748U)
        return;
    cout << "\nPassword was verified successfully." << endl;

    string currency_code;
    do
    {
        cout << "Please input a currency code with 3~6 characters : ";
        cin >> currency_code;
    } while (currency_code.length() < 3 && currency_code.length() > 6);

    client->publish_curency(currency_code);
    cout << color::GREEN << "Published " << currency_code << color::RESET << endl;

    client->register_currency(currency_code,
                              1,
                              2,
                              false,
                              1000000,
                              100);

    cout << "Registered " << currency_code << endl;

    client->add_currency_for_designated_dealer(currency_code,
                                               TESTNET_DD_ADDRESS);
    cout << "Added currency " << currency_code << " for DD account, " << endl;

    client->mint_currency_for_designated_dealer(currency_code,
                                                0,
                                                TESTNET_DD_ADDRESS,
                                                1000000 * MICRO_COIN,
                                                3);
    cout << "Minted 1,000,000  " << currency_code << " to DD account " << endl;
}

void mint_currency(client_ptr client)
{
    cout << color::RED << "mint currencies ..." << color::RESET << endl;

    auto accounts = client->get_all_accounts();

    for (auto currency_code : currency_codes)
    {
        cout << currency_code << endl;
    }

    while (true)
    {
        string currency_code;
        uint64_t amount;
        cout << "Pleae input currency code and amout : ";

        cin >> currency_code;
        if (currency_code == "q" || currency_code == "quit")
            break;

        cin >> amount;

        try_catch([&]() {
            client->mint_currency_for_designated_dealer(currency_code,
                                                        0,
                                                        TESTNET_DD_ADDRESS,
                                                        amount * MICRO_COIN,
                                                        3);
            cout << "minted " << amount << " " << currency_code << " to DD account " << endl;
        });
    }
}

void update_dual_attestation_limit(client_ptr client)
{
    cout << color::GREEN << "Update dual attestation limit ..." << color::RESET << endl;

    check_password();

    uint64_t limit;
    cout << "Pleae input dual attestation limit amount : ";
    cin >> limit;

    client->update_dual_attestation_limit(0, limit * MICRO_COIN);

    cout << "succeded to update dual attestation limit to "
         << color::GREEN << limit << color::RESET << "."
         << endl;
}

void deploy_exchange(client_ptr client)
{
    cout << color::RED << "Deploy Exchange and initialize it ..." << color::RESET << endl;
    string mnemonic = "mnemonic/exchange.mne";

    client->recover_wallet_accounts(mnemonic);
    cout << "Violas client is using mnemonic file "
         << color::GREEN << mnemonic << color::RESET
         << endl;
    

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
        client->create_designated_dealer_ex("VLS",
                                            0,
                                            EXCHANGE_ADMIN_ADDRESS,
                                            admin_account.auth_key, //only auth key prefix is applied
                                            "Exchange Administrator",
                                            "www.violas.io",
                                            admin_account.pub_key,
                                            true);

        client->create_parent_vasp_account("VLS",
                                           0,
                                           user1_account.address,
                                           user1_account.auth_key,
                                           "Exchange user1",
                                           "www.violas.io",
                                           user1_account.pub_key,
                                           true);

        client->create_parent_vasp_account("VLS",
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

        cout << "VLS : " // << double(client->get_balance(addr)) / MICRO_COIN << ", "
             << "USD : " << fmt_balance("VLSUSD") << ", "
             << "EUR : " << fmt_balance("VLSEUR") << "."
             << "GBP : " << fmt_balance("VLSGBP") << "."
             << endl;
    };

    //print_all_balance(accounts[0].address);
    //print_all_balance(accounts[1].address);

    //////////////////////////////////////////////////////////////////////////////////////////
    const Address reward_admin_address = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x56, 0x4C, 0x53, 0x01}; //000000000000000000000000564C5301
    const string script_path = "move/exchange/";
    auto exchange = Exchange::create(client, script_path);

    try_catch([&]() {
        exchange->deploy_with_root_account();
        cout << color::GREEN << "deploied Exchange contracts on Violas blockchain." << color::RESET << endl;

        exchange->initialize(admin, reward_admin_address);
        cout << color::GREEN << "Initialize Exchange contracts with admin account." << color::RESET << endl;

        for (auto currency : currency_codes)
        {
            exchange->add_currency(currency);
        }
        cout << color::GREEN << "add all currencies for Exchange" << color::RESET << endl;
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
    const string mnemonic = "mnemonic/bank.mne";

    client->recover_wallet_accounts(mnemonic);
    cout << "Violas client is using mnemonic file "
         << color::GREEN << mnemonic << color::RESET
         << endl;

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
        client->create_designated_dealer_ex("VLS",
                                            0,
                                            BANK_ADMIN_ADDRESS,
                                            admin_account.auth_key, //only auth key prefix is applied
                                            "Bank Administrator",
                                            "www.violas.io",
                                            admin_account.pub_key,
                                            true);

        client->create_parent_vasp_account("VLS",
                                           0,
                                           user1_account.address,
                                           user1_account.auth_key,
                                           "Bank user1",
                                           "www.violas.io",
                                           user1_account.pub_key,
                                           true);

        client->create_parent_vasp_account("VLS",
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

    auto bank = Bank::create_bank(client, "move/bank/"); //../../move/bank/
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
                               Bank::MANTISSA_1_0 / 5,       // 20%
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

void create_bridge_accounts(client_ptr client)
{
    // address, type, name of Bridge account
    static tuple<Address, account_type, string> bridge_address_type[] = {
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 'B', 'R', 'G', 'B', 'U', 'R', 'N'}, DD, "Bridge Burn"},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 'B', 'R', 'G', 'F', 'U', 'N', 'D'}, DD, "Bridge Fund"},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 'B', 'R', 'G', 'U', 'S', 'D', 'T'}, DD, "Bridge USDT"},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 'B', 'R', 'G', '-', 'B', 'T', 'C'}, DD, "Bridge BTC"},
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, VASP, "Bridge Parent VASP"},
    };

    string mnemonic = "mnemonic/bridge.mne";

    client->recover_wallet_accounts(mnemonic);
    cout << "Violas client is using mnemonic file "
         << color::GREEN << mnemonic << color::RESET
         << endl;

    try
    {
        for (const auto &[address, type, name] : bridge_address_type)
        {
            auto [_, index] = client->create_next_account(type == DD ? optional(address) : nullopt);
            auto accounts = client->get_all_accounts();
            const auto &account = accounts[index];

            cout << "Account " << index << ", address : " << account.address << ", authentication key : " << account.auth_key << endl;

            switch (type)
            {
            case DD:
            {
                client->create_designated_dealer_ex("VLS", 0, account.address, account.auth_key, name, "", account.pub_key, true);
            }
            break;
            case VASP:
            {
                client->create_parent_vasp_account("VLS", 0, account.address, account.auth_key, name, "", account.pub_key, true);
            }
            break;
            default:
                break;
            }

            for (auto currency : currency_codes)
            {
                client->add_currency(index, currency);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    //
    //  Create parent VASP account for backend(VLS-USER)
    //
    mnemonic = "mnemonic/backend.mne";
    client->recover_wallet_accounts(mnemonic);
    cout << "Violas client is using mnemonic file "
         << color::GREEN << mnemonic << color::RESET
         << endl;

    auto [address, index] = client->create_next_account();
    auto accounts = client->get_all_accounts();
    const auto &account = accounts[index];

    try
    {
        client->create_parent_vasp_account("VLS", 0, account.address, account.auth_key, "VLS-USER", "", account.pub_key, true);

        for (auto currency : currency_codes)
        {
            client->add_currency(index, currency);
        }

        cout << "Account " << index << ", address : " << account.address << ", authentication key : " << account.auth_key << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void rotate_authentication_key(client_ptr client)
{
    client->create_next_account();
    auto accounts = client->get_all_accounts();

    client->rotate_authentication_key_with_nonce(VIOLAS_ROOT_ACCOUNT_ID, 0, accounts[0].auth_key);

    client->rotate_authentication_key_with_nonce(VIOLAS_TREASURY_COMPLIANCE_ACCOUNT_ID, 0, accounts[0].auth_key);

    client->rotate_authentication_key_with_nonce(VIOLAS_TESTNET_DD_ACCOUNT_ID, 0, accounts[0].auth_key);

    cout << "succeeded to rotate mint key." << endl;

    client->save_private_key(0, "./mint.key");
    cout << "saved mint.key to current path." << endl;
}