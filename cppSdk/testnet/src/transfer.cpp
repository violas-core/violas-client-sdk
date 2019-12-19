#include <iostream>
#include <fstream>
#include <string_view>
#include <assert.h>
#include "libra_client.hpp"

using namespace std;
using namespace Violas;

//账户数量
const size_t ACCOUNT_NUM = 7;

//6种稳定币
const vector<string> STABLE_TOKEN_NAMES = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};

void deploy(Violas::client_ptr client);
void publish(Violas::client_ptr client);
void transfer(Violas::client_ptr client);
void transfer_libra(Violas::client_ptr client);

const std::string RED("\033[0;31m");
const std::string GREEN("\033[1;32m");
const std::string YELLOW("\033[1;33m");
const std::string CYAN("\033[0;36m");
const std::string MAGENTA("\033[0;35m");
const std::string RESET("\033[0m");

int main(int argc, char *argv[])
{
    ofstream file("log.txt");
    streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        cout << "usage: \n"
             << "\t"
             << GREEN << "transfer host port mnemonic [faucet]" << RESET
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

        COUT << "connecting to " << host << ":" << port << " ......"
             << endl;

        auto client = Violas::Client::create(host,
                                             port,
                                             "consensus_peers.config.toml",
                                             faucet_key,
                                             false,
                                             "faucet.testnet.libra.org", //libra testnet use this url to get test libra coin
                                             mnemonic);

        client->test_validator_connection();

        COUT << "connected to validator with "
             << RED << "【 " << mnemonic << " 】" << RESET
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
                 << RED << "\t【 " << mnemonic << " 】" << RESET << "\n"
                 << GREEN
                 << "\t1. Transfer Stable Token \n"
                 << "\t2. Transfer VToken \n"
                 << "\t3. Publish for a token\n"
                 << "\t4. Deploy Stable Token \n"
                 << "\t5. Quit \n"
                 << RESET
                 << "Please input the index : ";

            cin >> index;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }

            switch (index)
            {
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
        std::cerr << e.what() << '\n';
    }

    clog.rdbuf(mylog);

    return 0;
}

void deploy(Violas::client_ptr client)
{
    // client->mint_coins(0, 10);

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
    cout << "Please input tokenh address : ";
    cin >> token_address;

    auto token = Violas::Token::create(client, uint256_from_string(token_address), "Unkonw");
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

    auto receiver = uint256_from_string(address);

    //client->mint\\\_coins(0, amount);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;
    COUT << "Transferring " << amount << " cions ..." << endl;

    client->transfer_coins_int(0, receiver, amount * MICRO_LIBRO_COIN);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;
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

    auto &token1 = tokens[token_index];
    auto receiver = uint256_from_string(address);
    //
    // mint coins to the receiver
    //
    auto account_index = token_index + 1;

    token1->mint(account_index, receiver, amount * MICRO_LIBRO_COIN);

    auto micro_to_double = [](uint64_t amount) -> double {
        if (is_valid_balance(amount))
            return (double)amount / MICRO_LIBRO_COIN;
        else
            return 0;
    };

    auto balance_to_string = [](uint64_t value) -> string {
        if (is_valid_balance(value))
            return to_string(value);
        else
            return "N/A";
    };

    cout.precision(10);

    auto balance = token1->get_account_balance(receiver);
    cout << "account " << receiver << "'s balance is "
         << GREEN
         << balance_to_string(balance) << "(" << micro_to_double(balance) << ")"
         << RESET
         << endl;

    auto [txn, events] = client->get_committed_txn_by_acc_seq(account_index, client->get_sequence_number(account_index) - 1);

    CLOG << "Committed Transaction : \n"
         << txn << endl
         << "Events:\n"
         << events << endl;
}
