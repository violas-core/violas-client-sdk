#include <iostream>
#include <fstream>
#include <string_view>
#include <map>
#include <functional>
#include <assert.h>
#include <cxxabi.h>
#include "violas_sdk.hpp"
#include "terminal.h"

using namespace std;
using namespace Violas;

//账户数量
const size_t ACCOUNT_NUM = 5;

//6种稳定币
const vector<string> STABLE_TOKEN_NAMES = {"AAAUSD", "BBBGBP", "CCCEUR", "DDDJPY", "EEESGD", "DEFHKD"};

void mint(Violas::client_ptr client);
void deploy(Violas::client_ptr client);
void publish(Violas::client_ptr client);
void transfer(Violas::client_ptr client);
void transfer_libra(Violas::client_ptr client);

const char *demangle(const char *mangled_name)
{
    int status;

    return abi::__cxa_demangle(mangled_name, 0, 0, &status);
}

void create_token(client_ptr client, string script_files_path);

void transfer_token(client_ptr client, string script_files_path);

void deploy_tokens(client_ptr client, string script_files_path);

int main(int argc, char *argv[])
{
    ofstream file("log.txt");
    streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        cout << "usage: \n"
             << "\t"
             << color::GREEN << "transfer host port mnemonic [faucet]" << color::RESET
             << "\n"
             << endl;

        auto host = "52.151.2.66";
        uint16_t port = 40001;
        string mnemonic = "mnemonic";
        string faucet_key;
        string scripts_path = "../../cppSdk/scripts";

        if (argc >= 2)
            host = argv[1];

        if (argc >= 3)
            port = stol(argv[2]);

        if (argc >= 4)
            mnemonic = argv[3];

        if (argc >= 5)
            faucet_key = argv[4];

        if (argc >= 6)
            scripts_path = argv[5];

        cout << color::RED << "running test for violas sdk ..." << color::RESET << endl;

        auto client = Client::create(host, port, faucet_key, true, "", mnemonic);

        client->test_validator_connection();
        cout << "succeed to test validator connection." << endl;

        using handler = function<void()>;
        map<int, handler> handlers = {
            {1, [=]() { transfer_libra(client); }},
            {2, [=]() { transfer_token(client, scripts_path); }},
            {3, [=]() { create_token(client, scripts_path); }},
            {4, [=]() { deploy_tokens(client, scripts_path); }},
        };

        int index;
        cout << "commands list : \n"
                "   1 - Transfer Violas coin\n"
                "   2 - Transfer Token coin\n"
                "   3 - Create a new Token\n"
                "   4 - Deploy 5 tokens\n"
                "input command index : ";
        cin >> index;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << "caught an excpetion, '" << e.what() << "'\n"; //typeid(e).name()
    }
    catch (...)
    {
        std::cout << "caught a unknonw exception with type "
                  << demangle(abi::__cxa_current_exception_type()->name())
                  << std::endl;
    }

    clog.rdbuf(mylog);

    return 0;
}

void transfer_libra(Violas::client_ptr client)
{
    cout << "Let's transfer VToken ..." << endl;

    auto [supervisor, sup_address] = client->create_next_account(true);
    cout << "Account 0's address is " << sup_address.to_string() << endl;

    string address;
    cout << "\tPlease input receiver address : ";
    cin >> address;

    uint64_t amount;
    cout << "\tPlease input amount of token : ";
    cin >> amount;

    auto receiver = Address::from_string(address);

    cout << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;

    client->mint_coins(0, amount);

    auto [accout_index, sequence] = client->transfer_coins_int(0, receiver, amount * MICRO_COIN);

    cout << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;

    //auto [txn, events] = client->get_committed_txn_by_acc_seq(accout_index, sequence);
}

void deploy_tokens(client_ptr client, string script_files_path)
{
    cout << "Are you sure to deploy 5 tokens ? y for yes, other for no " << endl;

    char c;
    cin >> c;
    if (c != 'y')
        return;

    cout << "Let's deploy 5 tokens ..." << endl;

    auto [supervisor, sup_address] = client->create_next_account(true);

    client->mint_coins(supervisor, 1);

    auto token_mgr = TokenManager::create(client, sup_address, "token1", script_files_path);
    token_mgr->deploy(supervisor);
    token_mgr->publish(supervisor);

    for (size_t i = 0; i < ACCOUNT_NUM; i++)
    {
        auto [index, address] = client->create_next_account(true);
        client->mint_coins(index, 1);

        token_mgr->create_token(supervisor, address, STABLE_TOKEN_NAMES[i]);
        token_mgr->publish(index);

        token_mgr->mint(i, index, address, 1 * MICRO_COIN);

        cout << "Token index : " << i
             << ", owner's address : " << address
             << " , owner's balance is " << (double)token_mgr->get_account_balance(i, address) / MICRO_COIN
             << endl;
    }
}

void create_token(client_ptr client, string script_files_path)
{
    cout << "Let's create a new token ..." << endl;

    auto s = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        client->mint_coins(account.index, 1);

        cout << "Account index : " << account.index
             << ", address : " << account.address
             << ", balane : " << client->get_balance(account.index)
             << endl;
    }

    uint64_t supervisor = 0;

    auto token_mgr = TokenManager::create(client, accounts[supervisor].address, "token1", script_files_path);

    token_mgr->deploy(supervisor);
    token_mgr->publish(supervisor);

    string token_addr, token_name;

    cout << "Please input a token address : ";
    cin >> token_addr;

    cout << "Please input a token name : ";
    cin >> token_name;

    auto owner = Address::from_string(token_addr);

    token_mgr->create_token(supervisor, owner, token_name);

    auto seq_num = client->get_sequence_number(supervisor);

    auto [txn, event] = client->get_committed_txn_by_acc_seq(supervisor, seq_num - 1);
    cout << "The txn for create_token : " << txn << endl;

    for (const auto &account : accounts)
        token_mgr->publish(account.index);

    cout << "finished create token." << endl;
}

void transfer_token(client_ptr client, string script_files_path)
{
    using namespace Violas;

    auto [supervisor, sup_address] = client->create_next_account(true);

    client->mint_coins(supervisor, 1);

    auto token_mgr = TokenManager::create(client, sup_address, "token1", script_files_path);

    for (size_t i = 0; i < ACCOUNT_NUM; i++)
    {
        auto [index, address] = client->create_next_account(true);

        cout << "Token index : " << i
             << ", owner address : " << address
             << ", Owner's balance is " << (double)token_mgr->get_account_balance(i, address) / MICRO_COIN
             << endl;
    }

    auto print_txn = [client](uint64_t account_index) {
        auto seq_num = client->get_sequence_number(account_index) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(account_index, seq_num);
        cout << "txn = " << txn << endl;
    };

    do
    {
        string input;
        uint64_t amount = 0;
        uint64_t token_index = 0;

        cout << "Please input token index : ";
        cin >> token_index;
        if (token_index > 5)
            throw runtime_error("token index must be less than 5");

        cout << "Please input an address of receiver : ";
        cin >> input;
        auto receiver = Address::from_string(input);

        cout << "Pleae input amount : ";
        cin >> amount;

        uint64_t account_index = token_index + 1;

        cout << "The original balance of token " << STABLE_TOKEN_NAMES[token_index]
             << " of receiver " << receiver.to_string() << " is "
             << (double)token_mgr->get_account_balance(token_index, receiver) / MICRO_COIN
             << endl;

        token_mgr->mint(token_index, account_index, receiver, amount * MICRO_COIN);

        cout << "The current balance of token " << STABLE_TOKEN_NAMES[token_index]
             << " of receiver " << receiver.to_string() << " is "
             << (double)token_mgr->get_account_balance(token_index, receiver) / MICRO_COIN
             << endl;
    } while (true);
}
