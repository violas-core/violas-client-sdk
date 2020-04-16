#include <iostream>
#include <fstream>
#include <string_view>
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

void transfer_token(string host, short port, string mnemonic_file, string mint_key_file);

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

        if (argc >= 2)
            host = argv[1];

        if (argc >= 3)
            port = stol(argv[2]);

        if (argc >= 4)
            mnemonic = argv[3];

        if (argc >= 5)
            faucet_key = argv[4];

        transfer_token(host, port, mnemonic, faucet_key);

        return 0;

        COUT << "connecting to " << host << ":" << port << " ......"
             << endl;

        auto client = Violas::Client::create(host,
                                             port,
                                             "consensus_peers.config.toml",
                                             faucet_key,
                                             false,
                                             "", //libra testnet use this url to get test libra coin
                                             mnemonic);

        client->test_validator_connection();

        COUT << "connected to validator with "
             << color::RED << "【 " << mnemonic << " 】" << color::RESET
             << endl;

        //
        //  Create all accounts
        //

        for (uint64_t i = 0; i < ACCOUNT_NUM; i++)
        {
            client->create_next_account(true);
        }

        COUT << "List all available acounts" << endl;
        auto accounts = client->get_all_accounts();

        for (auto const &account : accounts)
        {
            uint64_t balance = client->get_balance(account.index);

            cout << "\n\tIndex : " << account.index
                 << "\n\tAddress : " << account.address
                 << "\n\tSequence : " << account.sequence_number
                 << "\n\tStatus : " << account.status
                 << "\n\tToken Balance : " << balance
                 << endl;
        }

        size_t index = 0;

        while (index < 5)
        {
            COUT << "Index for functions \n"
                 << color::RED << "\t【 " << mnemonic << " 】" << color::RESET << "\n"
                 << color::GREEN
                 << (faucet_key.empty() ? "" : "\t0. Mint VToken to account 0 \n")
                 << "\t1. Transfer Stable Token \n"
                 << "\t2. Transfer VToken \n"
                 << "\t3. Publish for a token\n"
                 << "\t4. Deploy Stable Token \n"
                 << "\t5. Quit \n"
                 << color::RESET
                 << "Please input the index : ";

            cin >> index;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                continue;
            }

            switch (index)
            {
            case 0:
                mint(client);
                break;
            case 1:
                transfer(client);
                break;
            case 2:
                transfer_libra(client);
                break;
            case 3:
                publish(client);
                break;
            case 4:
                deploy(client);
                break;
            default:
                break;
            }

            // cin >> index;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "catch an " << typeid(e).name() << " excpetion, '" << e.what() << "'\n";
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

void deploy(Violas::client_ptr client)
{
    auto accounts = client->get_all_accounts();

    uint64_t balance = client->get_balance(1);
    if (balance < 1.0f)
    {
        // make the all accounts have the enough VToken(libra) for executing the move program
        for (auto const &account : accounts)
        {
            client->transfer_coins_int(0, account.address, 1 * MICRO_LIBRO_COIN);
        }
    }

    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= STABLE_TOKEN_NAMES.size() + 1);

    COUT << "Deploying ";
    for (size_t i = 0; i < STABLE_TOKEN_NAMES.size(); i++)
    {
        cout << ".";
        cout.flush();

        auto vstake = Violas::Token::create(client, accounts[i + 1].address, STABLE_TOKEN_NAMES.at(i));

        vstake->deploy(i + 1);

        vstake->publish(i + 1);

        tokens.push_back(vstake);
    }
    cout << endl;

    COUT << "List all tokens :" << endl;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        auto &token = tokens[i];
        cout << "\tToken " << i << "'s name is " << token->name() << ", address is " << token->address() << endl;
    }

    cout << "Findished deploying 6 Tokens" << endl;
}

void publish(Violas::client_ptr client)
{
    auto accounts = client->get_all_accounts();

    string token_address;
    cout << "Please input token's address : ";
    cin >> token_address;

    auto token = Violas::Token::create(client, Address::from_string(token_address), "Unkonw");
    cout << "Current token's address : " << token->address() << endl;

    cout << "List all accounts :" << endl;
    for (auto &account : accounts)
    {
        cout << "\taccount  "
             << "index : " << account.index
             << "address : " << account.address << endl;
    }

    //
    //  get all arguments
    //
    size_t account_index;
    cout << "Please input accout index for publish : ";
    cin >> account_index;

    token->publish(account_index);
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

void transfer(Violas::client_ptr client)
{
    auto accounts = client->get_all_accounts();

    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= STABLE_TOKEN_NAMES.size() + 1);

    for (size_t i = 0; i < STABLE_TOKEN_NAMES.size(); i++)
    {
        auto token = Violas::Token::create(client, accounts[i + 1].address, STABLE_TOKEN_NAMES.at(i));

        tokens.push_back(token);
    }

    cout << "List all tokens :" << endl;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        auto &token = tokens[i];
        cout << "\tToken " << i << "'s name is " << token->name() << ", address is " << token->address() << endl;
    }
    //
    //  get all arguments
    //
    size_t token_index;
    cout << "Please input Token index : ";
    cin >> token_index;

    string address;
    cout << "Please input receiver address : ";
    cin >> address;

    uint64_t amount;
    cout << "Please input amount of token : ";
    cin >> amount;

    auto &token = tokens[token_index];
    auto receiver = Address::from_string(address);
    //
    // mint coins to the receiver
    //
    auto account_index = token_index + 1;

    // token->mint(account_index, receiver, amount * MICRO_LIBRO_COIN);

    // auto micro_to_double = [](uint64_t amount) -> double {
    //     if (is_valid_balance(amount))
    //         return (double)amount / MICRO_LIBRO_COIN;
    //     else
    //         return 0;
    // };

    // auto balance_to_string = [](uint64_t value) -> string {
    //     if (is_valid_balance(value))
    //         return to_string(value);
    //     else
    //         return "N/A";
    // };

    // cout.precision(10);

    // auto balance = token->get_account_balance(receiver);
    // cout << "account " << receiver << "'s balance is "
    //      << GREEN
    //      << balance_to_string(balance) << "(" << micro_to_double(balance) << ")"
    //      << RESET
    //      << endl;

    auto [txn, events] = client->get_committed_txn_by_acc_seq(account_index, client->get_sequence_number(account_index) - 1);

    CLOG << "Committed Transaction : \n"
         << txn << endl
         << "Events:\n"
         << events << endl;
}

void transfer_token(string host, short port, string mnemonic_file, string mint_key_file)
{
    using namespace Violas;
    
    cout << color::RED << "running test for violas sdk ..."  << color::RESET << endl;

    auto client = Client::create(host, port, "", mint_key_file, true, "", mnemonic_file);

    client->test_validator_connection();
    cout << "succeed to test validator connection ." << endl;

    auto s = client->create_next_account(true);
    auto o1 = client->create_next_account(true);
    auto o2 = client->create_next_account(true);
    auto u1 = client->create_next_account(true);
    auto u2 = client->create_next_account(true);

    auto accounts = client->get_all_accounts();
    for (const auto &account : accounts)
    {
        client->mint_coins(account.index, 1);

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

    string script_files_path = "../../cppSdk/scripts";
    auto token = Token::create(client, accounts[supervisor].address, "token1", script_files_path);

    token->deploy(supervisor);
    //print_txn(0);
    cout << "account "<< supervisor <<" deployed token successfully ." << endl;

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
    token->mint(owner1, 0, accounts[user1].address, 100);
    //print_txn(owner1);
    cout << "owner1 mint 100 cions to user1 ." << endl;

    auto balance = token->get_account_balance(user1, 0);
    cout << "the balance of token A of user 1 is " << balance << endl;

    //
    // 7. Ob调用mint给U2铸Tb币种的100块钱
    //
    token->mint(owner2, 1, accounts[user2].address, 100);
    //print_txn(owner2);
    cout << "Owner2 mint 100 coins to user2" << endl;

    balance = token->get_account_balance(user2, 1);
    cout << "the balance of token B of user 1 is " << balance << endl;


    token->transfer(user1, 0, accounts[user2].address, 50);
    //print_txn(owner1);
    balance = token->get_account_balance(user2, 0);
    cout << "User 1 transferred 50 token A to user 2, the balance of Token A fo user 2 is " << balance << endl;

    token->transfer(user2, 1, accounts[user1].address, 50);
    //print_txn(owner2);
    balance = token->get_account_balance(user1, 1);
    cout << "User 2 transferred 50 token B to user 1, the balance of Token B of user 1 is " << balance << endl;


    cout << "User 1's token A : " << token->get_account_balance(user1, 0) << "\n"
         << "User 1's token B : " << token->get_account_balance(user1, 1) << "\n"
         << "User 2's token A : " << token->get_account_balance(user2, 0) << "\n"
         << "User 2's token B : " << token->get_account_balance(user2, 1) << endl;

    string input;
    cout << "Please input an address of receiver : ";
    cin >> input;
    cout << endl << input << endl;

    auto receiver = Address::from_string(input); //7f4644ae2b51b65bd3c9d414aa853407
    cout << "receiver address is " << receiver << endl;

    token->mint(owner1, 0, receiver, 1000000);
    print_txn(owner1);
    balance = token->get_account_balance(0, receiver);
    cout << "the balance of token A receiver is " << balance << endl;

    token->mint(owner2, 1, receiver, 2000000);
    print_txn(owner2);
    balance = token->get_account_balance(1, receiver);
    cout << "the balance of token B of receiver is " << balance << endl;
}
