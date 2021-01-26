#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <map>
#include <functional>
#include <client.hpp>
#include "argument.hpp"
#include "utils.h"

using namespace std;
using namespace violas;

void initialize_swap(client_ptr client);
void test_swap(client_ptr client);

const auto USD = make_currency_tag("USD");
const auto EUR = make_currency_tag("EUR");
const auto GBP = make_currency_tag("GBP");

int main(int argc, char *argv[])
{
    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);
        args.show();

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemoic, args.waypoint);

        client->test_connection();

        Address admin_address = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xEE};

        client->create_next_account(admin_address);
        client->create_next_account();
        client->create_next_account();

        auto accounts = client->get_all_accounts();
        for (const auto &account : accounts)
        {
            cout << "Account index : " << account.index
                 << ", address : " << account.address
                 << ", auth key : " << account.auth_key
                 << endl;
        }

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { initialize_swap(client); }},
            {2, [=]() { test_swap(client); }},
        };

        cout << "1 for initialize_swap \n"
             << "2 for test_swap" << endl;
        
        int index;
        cin >> index;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void register_currency(client_ptr client, string_view currency_code)
{
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

void initialize_swap(client_ptr client)
{
    client->allow_publishing_module(true);
    client->allow_custom_script();

    register_currency(client, "USD");
    register_currency(client, "EUR");

    auto accounts = client->get_all_accounts();
    const auto &admin = accounts[0];

    client->create_designated_dealer_ex("VLS", 0, admin.address, admin.auth_key, "EE", "", admin.pub_key, true);
    client->add_currency(0, "USD");
    client->add_currency(0, "EUR");

    client->create_parent_vasp_account("VLS", 0, accounts[1].address, accounts[1].auth_key, "EE01", "", admin.pub_key, true);
    client->add_currency(1, "USD");
    client->add_currency(1, "EUR");

    client->mint_for_testnet("USD", accounts[1].address, 100 * MICRO_COIN);
    client->mint_for_testnet("EUR", accounts[1].address, 100 * MICRO_COIN);

    client->create_parent_vasp_account("VLS", 0, accounts[2].address, accounts[2].auth_key, "EE01", "", admin.pub_key, true);
    client->add_currency(2, "USD");
    client->add_currency(2, "EUR");

    client->mint_for_testnet("USD", accounts[2].address, 100 * MICRO_COIN);

    client->mint_for_testnet("EUR", accounts[2].address, 100 * MICRO_COIN);

    client->publish_module(VIOLAS_ROOT_ACCOUNT_ID, "move/swap/exchange.mv");
    cout << "publish swap.mv" << endl;

    client->execute_script_file(admin.index, "move/swap/initialize.mv");
    cout << "exec initialize.mv" << endl;

    client->execute_script_file(admin.index, "move/swap/add_reserve.mv", {USD, EUR});
    cout << "exec add_reserve.mv" << endl;
}

void test_swap(client_ptr client)
{
    auto accounts = client->get_all_accounts();
    const auto &admin = accounts[0];

    client->execute_script_file(admin.index, "move/swap/verify_sqrt.mv");
    cout << "exec verify_sqrt.mv" << endl;    

    client->execute_script_file(1, "move/swap/deposit_liquidity.mv", {USD, EUR}, {2 * MICRO_COIN, 18 * MICRO_COIN});
    cout << "exec deposit_liquidity.mv" << endl;

    client->execute_script_file(2, "move/swap/swap.mv", {USD, EUR}, {1 * MICRO_COIN, 6 * MICRO_COIN});    
    cout << "exec swap.mv" << endl;

    client->execute_script_file(1, "move/swap/withdraw_liquidity.mv", {USD, EUR}, {6 * MICRO_COIN});
    cout << "exec withdraw_liquidity.mv" << endl;

    // client->execute_script_file(1, "move/swap/deposit_liquidity.mv", {USD, EUR}, {2 * MICRO_COIN, 18 * MICRO_COIN});
    // cout << "exec deposit_liquidity.mv" << endl;
    
    // client->execute_script_file(2, "move/swap/swap.mv", {EUR, USD}, {6 * MICRO_COIN, uint64_t(0 * MICRO_COIN)});
    // cout << "exec swap.mv" << endl;

    // client->execute_script_file(1, "move/swap/withdraw_liquidity.mv", {USD, EUR}, {6 * MICRO_COIN});
    // cout << "exec withdraw_liquidity.mv" << endl;
}