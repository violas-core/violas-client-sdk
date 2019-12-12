#include <iostream>
#include <fstream>
#include <string_view>
#include <assert.h>
#include "libra_client.hpp"

using namespace std;
using namespace Violas;

//6种稳定币
const vector<string> names = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};

void deploy(Violas::client_ptr client);
void transfer(Violas::client_ptr client);
void transfer_libra(Violas::client_ptr client);

int main(int argc, char *argv[])
{
    ofstream file("log.txt");
    streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        cout << "usage: \n"
             << "\t" << "transfer host port" << "\n"
             << endl;

        auto host = "52.151.2.66";
        uint16_t port = 40001;

        if (argc >= 3)
        {
            host = argv[1];
            port = stol(argv[2]);
        }

        auto client = Violas::Client::create(host,
                                             port,
                                             "consensus_peers.config.toml",
                                             "temp_faucet_keys",
                                             false,
                                             "faucet.testnet.libra.org", //libra testnet use this url to get test libra coin
                                             "mnemonic.bak");

        client->test_validator_connection();
        COUT << "succeeded to connect validator " << host << ":" << port << endl;

        COUT << "Index for functions \n"
             << "\t1. Transfer Stable Token \n"
             << "\t2. Transfer VToken \n"
             << "\t3. Deploy Stable Token \n"
             << "\t4. Quit \n"
             << "Please input the index : ";
        //<< endl;

        size_t index;
        cin >> index;

        //while (index <= 3)
        {
            switch (index)
            {
            case 1:
                transfer(client);
                break;
            case 2:
                transfer_libra(client);
                break;
            case 3:
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

    const uint64_t account_amount = 8;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    client->mint_coins(0, 10);

    auto accounts = client->get_all_accounts();

    COUT << "List all available acounts" << endl;

    for (auto const &account : accounts)
    {

        client->transfer_coins_int(0, account.address, 1 * MICRO_LIBRO_COIN);

        uint64_t balance = client->get_balance(account.index);

        cout << "\n\tIndex : " << account.index
             << "\n\tAddress : " << account.address
             << "\n\tSequence : " << account.sequence_number
             << "\n\tStatus : " << account.status
             << "\n\tToken Balance : " << balance
             << endl;
    }

    //6个州长发行6种稳定币
    vector<string> names = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};
    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= names.size() + 1);

    for (size_t i = 0; i < names.size(); i++)
    {
        auto vstake = Violas::Token::create(client, accounts[i + 1].address, names.at(i));

        vstake->deploy(i + 1);

        vstake->publish(i + 1);

        tokens.push_back(vstake);
    }


    cout << "List all tokens :" << endl;
    for (auto i = 0; i < tokens.size(); i++)
    {
        auto &token = tokens[i];
        cout << "\tToken " << i << "'s name is " << token->name() << ", address is " << token->address() << endl;

        // account index 7 publish all tokens
        token->publish(7);
    }

    cout << "Findished deploying 6 Tokens" << endl;
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

    client->create_next_account(true);
    client->mint_coins(0, amount);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;
    COUT << "Transferring " << amount << " cions ..." << endl;

    client->transfer_coins_int(0, receiver, amount * MICRO_LIBRO_COIN);

    COUT << "The address " << receiver << "'s balance is " << client->get_balance(receiver) << endl;
}

void transfer(Violas::client_ptr client)
{
    const uint64_t account_amount = 8;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    auto accounts = client->get_all_accounts();

    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= names.size() + 1);

    for (size_t i = 0; i < names.size(); i++)
    {
        auto vstake = Violas::Token::create(client, accounts[i + 1].address, names.at(i));

        tokens.push_back(vstake);
    }

    cout << "List all tokens :" << endl;
    for (auto i = 0; i < tokens.size(); i++)
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
    cout << "account " << receiver << "'s balance is " << balance_to_string(balance) << "(" << micro_to_double(balance) << ")" << endl;

    auto [txn, events] = client->get_committed_txn_by_acc_seq(account_index, client->get_sequence_number(account_index) - 1);

    CLOG << "Committed Transaction : \n"
         << txn << endl
         << "Events:\n"
         << events << endl;
}
