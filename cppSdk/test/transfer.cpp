#include <iostream>
#include <string_view>
#include <assert.h>
#include "libra_client.hpp"

using namespace std;

//6个州长发行6种稳定币
const vector<string> names = {"ABCUSD", "HIJUDS", "XYZUSD", "BCDCAN", "CDESDG", "DEFHKD"};

void deploy(string_view host, uint16_t port);
void transfer(string_view host, uint16_t port);

int main(int argc, char *argv[])
{
    try
    {
        auto host = "52.27.228.84";
        uint16_t port = 40001;

        if (argc == 2 && string("deploy") == argv[1])
            deploy(host, port);
        else
            transfer(host, port);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void deploy(string_view host, uint16_t port)
{
    using namespace Violas;
    auto client = Client::create(host.data(),
                                 port,
                                 "violas_consensus_peers.config.toml",
                                 "temp_faucet_keys",
                                 false,
                                 "faucet.testnet.libra.org", //libra testnet use this url to get test libra coin
                                 "mnemonic.bak");

    client->test_validator_connection();

    const uint64_t account_amount = 7;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    client->mint_coins(0, 10);

    auto accounts = client->get_all_accounts();

    cout << "List all available acounts" << endl;

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
    }

    cout << "Findished deploying 6 Tokens" << endl;
}

void transfer(string_view host, uint16_t port)
{
    using namespace Violas;

    auto client = client::create(host.data(),
                                 port,
                                 "violas_consensus_peers.config.toml",
                                 "", //temp_faucet_keys
                                 false,
                                 "faucet.testnet.libra.org", //libra testnet use this url to get test libra coin
                                 "mnemonic.bak");

    client->test_validator_connection();

    const uint64_t account_amount = 7;
    for (uint64_t i = 0; i < account_amount; i++)
    {
        client->create_next_account(true);
    }

    //client->mint_coins(0, 10);

    auto accounts = client->get_all_accounts();

    cout << "List all available acounts" << endl;

    for (auto const &account : accounts)
    {
        //client->transfer_coins_int(0, account.address, 1 * MICRO_LIBRO_COIN);

        uint64_t balance = client->get_balance(account.index);
        // assert(balance > 0);

        cout << "\n\tIndex : " << account.index
             << "\n\tAddress : " << account.address
             << "\n\tSequence : " << account.sequence_number
             << "\n\tStatus : " << account.status
             << "\n\tToken Balance : " << balance
             << endl;
    }

    vector<Violas::token_ptr> tokens;

    assert(accounts.size() >= names.size() + 1);

    for (size_t i = 0; i < names.size(); i++)
    {
        auto vstake = Violas::Token::create(client, accounts[i + 1].address, names.at(i));

        //vstake->deploy(i + 1);

        //vstake->publish(i + 1);

        tokens.push_back(vstake);
    }

    cout << "List all tokens :" << endl;
    for (auto i = 0; i < tokens.size(); i++)
    {
        auto &token = tokens[i];
        cout << "\tToken " << i << "'s name is " << token->name() << ", address is " << token->address() << endl;
    }

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
    auto addr = uint256_from_string(address);

    auto account_index = token_index + 1;

    token1->mint(account_index, addr, amount * MICRO_LIBRO_COIN);

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

    auto balance = token1->get_account_balance(addr);
    cout << "account " << addr << "'s balance is " << balance_to_string(balance) << "(" << micro_to_double(balance) << ")" << endl;

    auto [txn, events] = client->get_committed_txn_by_acc_seq(account_index, client->get_sequence_number(account_index) - 1);

    LOG << "Committed Transaction : \n"
        << txn << endl
        << "Events:\n"
        << events << endl;
}
