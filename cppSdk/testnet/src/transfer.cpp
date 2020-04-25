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
const size_t ACCOUNT_NUM = 7;

//6种稳定币
const vector<string> STABLE_TOKEN_NAMES = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};

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
            {0, [=]() { create_token(client, scripts_path); }},
            {1, [=]() { transfer_token(client, scripts_path); }},
        };

        int index;
        cout << "input command index, 0 for create token, 1 for transfer : ";
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
void mint(Violas::client_ptr client)
{
    cout << "Minting VToken to account 0 \n"
         << "Please input the amount :";

    uint64_t amount;
    cin >> amount;

    cout << "Minting " << amount << " VToken ..." << endl;

    client->mint_coins(0, amount);

    cout << "Account 0's balance is " << client->get_balance(0) << endl;
}

void transfer_libra(Violas::client_ptr client)
{
    COUT << "Let's transfer VToken ..." << endl;

    string address;
    cout << "\tPlease input receiver address : ";
    cin >> address;

    uint64_t amount;
    cout << "\tPlease input amount of token : ";
    cin >> amount;

    auto receiver = Address::from_string(address);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;
    COUT << "Transferring " << amount << " cions ..." << endl;

    auto [accout_index, sequence] = client->transfer_coins_int(0, receiver, amount * MICRO_LIBRO_COIN);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;

    auto [txn, events] = client->get_committed_txn_by_acc_seq(accout_index, sequence);

    // cout << "txn :" << endl
    //      << txn << endl
    //      << "events :" << endl
    //      << events;
}

void create_token(client_ptr client, string script_files_path)
{
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

    token_mgr->publish(0);

    cout << "finished create token." << endl;
}

void transfer_token(client_ptr client, string script_files_path)
{
    using namespace Violas;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto o2 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        // client->mint_coins(account.index, 1);

        cout << "Account index : " << account.index
             << ", address : " << account.address
             << ", balane : " << client->get_balance(account.index)
             << endl;
    }

    auto print_txn = [client](uint64_t account_index) {
        auto seq_num = client->get_sequence_number(account_index) - 1;
        auto [txn, event] = client->get_committed_txn_by_acc_seq(account_index, seq_num);
        cout << "txn = " << txn << endl;
    };

    uint64_t supervisor = 0,
             owner1 = 1,
             owner2 = 2,
             user1 = 3,
             user2 = 4;

    auto token = TokenManager::create(client, accounts[supervisor].address, "token1", script_files_path);
    /*
    token->deploy(supervisor);
    //print_txn(0);
    cout << "account " << supervisor << " deployed token successfully ." << endl;

    token->publish(supervisor);
    //print_txn(0);
    cout << "supervisor published Violas Token Module successfuly." << endl;

    //token->create_token(supervisor, accounts[owner1].address, "Token A");
    //print_txn(0);
    cout << "created token A successfully." << endl;

    //token->create_token(supervisor, accounts[owner2].address, "Token B");
    //print_txn(0);
    cout << "created token B successfully." << endl;

    for (size_t i = 1; i < accounts.size(); i++)
    {
        token->publish((uint64_t)i);
        //print_txn(0);
    }
    cout << "all accounts publish token module successfully." << endl;
    
    //
    // 6. Oa调用mint给U1铸Ta币种的100块钱
    //
    token->mint(0, owner1, accounts[user1].address, 100);
    //print_txn(owner1);
    cout << "owner1 mint 100 cions to user1 ." << endl;

    auto balance = token->get_account_balance(0, user1);
    cout << "the balance of token A of user 1 is " << balance << endl;

    //
    // 7. Ob调用mint给U2铸Tb币种的100块钱
    //
    token->mint(0, owner2, accounts[user2].address, 100);
    //print_txn(owner2);
    cout << "Owner2 mint 100 coins to user2" << endl;

    balance = token->get_account_balance(1, user2);
    cout << "the balance of token B of user 1 is " << balance << endl;

    token->transfer(0, user1, accounts[user2].address, 50);
    //print_txn(owner1);
    balance = token->get_account_balance(0, user2);
    cout << "User 1 transferred 50 token A to user 2, the balance of Token A fo user 2 is " << balance << endl;

    token->transfer(1, user2, accounts[user1].address, 50);
    //print_txn(owner2);
    balance = token->get_account_balance(1, user1);
    cout << "User 2 transferred 50 token B to user 1, the balance of Token B of user 1 is " << balance << endl;

    cout << "User 1's token A : " << token->get_account_balance(0, user1) << "\n"
         << "User 1's token B : " << token->get_account_balance(1, user1) << "\n"
         << "User 2's token A : " << token->get_account_balance(0, user2) << "\n"
         << "User 2's token B : " << token->get_account_balance(1, user2) << endl;
    */
    do
    {
        string input;
        uint64_t amount = 0;
        uint64_t token_index = 0;
        map<uint64_t, uint64_t> token_owner = {{0, owner1}, {1, owner2}};

        cout << "Please inout token index : ";
        cin >> token_index;
        if (token_index > 1)
            throw runtime_error("token index must be less than 2");

        cout << "Please input an address of receiver : ";
        cin >> input;
        auto receiver = Address::from_string(input);

        cout << "Pleae input amount for address " << receiver << " : ";
        cin >> amount;

        token->mint(token_index, token_owner[token_index], receiver, amount * MICRO_LIBRO_COIN);
        //print_txn(owner1);
        auto balance = token->get_account_balance(token_index, receiver);
        cout << "the balance of token A of receiver is " << (double)balance / MICRO_LIBRO_COIN << endl;
    } while (true);
}
